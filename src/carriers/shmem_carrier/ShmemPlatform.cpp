/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ShmemPlatform.h"
#include "ShmemLogComponent.h"

#include <yarp/os/Log.h>

#include <cerrno>
#include <cstring>
#include <mutex>

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  include <windows.h>
#else
#  include <arpa/inet.h>
#  include <fcntl.h>
#  include <netdb.h>
#  include <netinet/in.h>
#  include <semaphore.h>
#  include <sys/mman.h>
#  include <sys/socket.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#  include <unistd.h>
#  if defined(__APPLE__)
#    include <chrono>
#    include <thread>
#  else
#    include <ctime>
#  endif
#endif

namespace {

#if defined(_WIN32)

using native_socket_t = SOCKET;
const native_socket_t invalid_socket = INVALID_SOCKET;

void ensureSocketInit()
{
    static std::once_flag flag;
    std::call_once(flag, []() {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    });
}

int closeNativeSocket(native_socket_t s)
{
    return closesocket(s);
}

bool isInterrupted()
{
    return false;
}

std::string lastError()
{
    return std::to_string(GetLastError());
}

std::string lastSocketError()
{
    return std::to_string(WSAGetLastError());
}

#else

using native_socket_t = int;
const native_socket_t invalid_socket = -1;

void ensureSocketInit()
{
}

int closeNativeSocket(native_socket_t s)
{
    return ::close(s);
}

bool isInterrupted()
{
    return errno == EINTR;
}

std::string lastError()
{
    return std::strerror(errno);
}

std::string lastSocketError()
{
    return std::strerror(errno);
}

#endif

native_socket_t toNative(std::intptr_t fd)
{
    return static_cast<native_socket_t>(fd);
}

std::intptr_t fromNative(native_socket_t s)
{
    return (s == invalid_socket) ? -1 : static_cast<std::intptr_t>(s);
}

bool addressToString(const sockaddr_in& addr, std::string& host, int& port)
{
    char buf[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, const_cast<in_addr*>(&addr.sin_addr), buf, sizeof(buf)) == nullptr) {
        return false;
    }
    host = buf;
    port = ntohs(addr.sin_port);
    return true;
}

} // namespace


std::string shmem::objectName(const char* kind, int port, int index)
{
    std::string name;
#if !defined(_WIN32)
    // POSIX names must start with a slash
    // (and must be shorter than 31 characters on macOS)
    name = "/";
#endif
    name += "yarp_shm_";
    name += kind;
    name += "_" + std::to_string(port);
    if (index >= 0) {
        name += "_" + std::to_string(index);
    }
    return name;
}


/******************************************************************************
 * SharedMemory
 ******************************************************************************/

shmem::SharedMemory::~SharedMemory()
{
    close();
}

#if defined(_WIN32)

bool shmem::SharedMemory::create(const std::string& name, size_t size)
{
    close();

    const auto sz = static_cast<unsigned long long>(size);
    HANDLE h = CreateFileMappingA(INVALID_HANDLE_VALUE,
                                  nullptr,
                                  PAGE_READWRITE,
                                  static_cast<DWORD>(sz >> 32),
                                  static_cast<DWORD>(sz & 0xFFFFFFFFULL),
                                  name.c_str());
    if (h == nullptr) {
        yCError(SHMEMCARRIER, "Cannot create shared memory %s (error %s)", name.c_str(), lastError().c_str());
        return false;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // The object is still in use by someone else, and it might be too small
        CloseHandle(h);
        yCError(SHMEMCARRIER, "Shared memory %s already exists", name.c_str());
        return false;
    }

    void* p = MapViewOfFile(h, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (p == nullptr) {
        yCError(SHMEMCARRIER, "Cannot map shared memory %s (error %s)", name.c_str(), lastError().c_str());
        CloseHandle(h);
        return false;
    }

    m_handle = h;
    m_data = p;
    m_size = size;
    return true;
}

bool shmem::SharedMemory::open(const std::string& name)
{
    close();

    HANDLE h = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name.c_str());
    if (h == nullptr) {
        yCError(SHMEMCARRIER, "Cannot open shared memory %s (error %s)", name.c_str(), lastError().c_str());
        return false;
    }

    void* p = MapViewOfFile(h, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (p == nullptr) {
        yCError(SHMEMCARRIER, "Cannot map shared memory %s (error %s)", name.c_str(), lastError().c_str());
        CloseHandle(h);
        return false;
    }

    MEMORY_BASIC_INFORMATION info;
    m_size = (VirtualQuery(p, &info, sizeof(info)) != 0) ? info.RegionSize : 0;
    m_handle = h;
    m_data = p;
    return true;
}

void shmem::SharedMemory::close()
{
    if (m_data != nullptr) {
        UnmapViewOfFile(m_data);
        m_data = nullptr;
    }
    if (m_handle != nullptr) {
        CloseHandle(static_cast<HANDLE>(m_handle));
        m_handle = nullptr;
    }
    m_size = 0;
}

void shmem::SharedMemory::unlink(const std::string& name)
{
    // Windows kernel objects are destroyed when the last handle is closed
    YARP_UNUSED(name);
}

#else // POSIX

bool shmem::SharedMemory::create(const std::string& name, size_t size)
{
    close();

    // Remove any stale segment left by a crashed process
    shm_unlink(name.c_str());

    int fd = shm_open(name.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        yCError(SHMEMCARRIER, "Cannot create shared memory %s (%s)", name.c_str(), lastError().c_str());
        return false;
    }

    if (ftruncate(fd, static_cast<off_t>(size)) != 0) {
        yCError(SHMEMCARRIER, "Cannot resize shared memory %s (%s)", name.c_str(), lastError().c_str());
        ::close(fd);
        shm_unlink(name.c_str());
        return false;
    }

    void* p = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ::close(fd);
    if (p == MAP_FAILED) {
        yCError(SHMEMCARRIER, "Cannot map shared memory %s (%s)", name.c_str(), lastError().c_str());
        shm_unlink(name.c_str());
        return false;
    }

    m_data = p;
    m_size = size;
    return true;
}

bool shmem::SharedMemory::open(const std::string& name)
{
    close();

    int fd = shm_open(name.c_str(), O_RDWR, 0);
    if (fd < 0) {
        yCError(SHMEMCARRIER, "Cannot open shared memory %s (%s)", name.c_str(), lastError().c_str());
        return false;
    }

    struct stat st;
    if (fstat(fd, &st) != 0 || st.st_size <= 0) {
        yCError(SHMEMCARRIER, "Cannot get the size of shared memory %s", name.c_str());
        ::close(fd);
        return false;
    }

    const auto size = static_cast<size_t>(st.st_size);
    void* p = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ::close(fd);
    if (p == MAP_FAILED) {
        yCError(SHMEMCARRIER, "Cannot map shared memory %s (%s)", name.c_str(), lastError().c_str());
        return false;
    }

    m_data = p;
    m_size = size;
    return true;
}

void shmem::SharedMemory::close()
{
    if (m_data != nullptr) {
        munmap(m_data, m_size);
        m_data = nullptr;
    }
    m_size = 0;
}

void shmem::SharedMemory::unlink(const std::string& name)
{
    shm_unlink(name.c_str());
}

#endif


/******************************************************************************
 * NamedSemaphore
 ******************************************************************************/

shmem::NamedSemaphore::~NamedSemaphore()
{
    close();
}

#if defined(_WIN32)

bool shmem::NamedSemaphore::create(const std::string& name, unsigned int initial, unsigned int max)
{
    close();

    HANDLE h = CreateSemaphoreA(nullptr, static_cast<LONG>(initial), static_cast<LONG>(max), name.c_str());
    if (h == nullptr) {
        yCError(SHMEMCARRIER, "Cannot create semaphore %s (error %s)", name.c_str(), lastError().c_str());
        return false;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // The object is still in use by someone else, its count is unknown
        CloseHandle(h);
        yCError(SHMEMCARRIER, "Semaphore %s already exists", name.c_str());
        return false;
    }
    m_handle = h;
    return true;
}

bool shmem::NamedSemaphore::open(const std::string& name)
{
    close();

    HANDLE h = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, name.c_str());
    if (h == nullptr) {
        yCError(SHMEMCARRIER, "Cannot open semaphore %s (error %s)", name.c_str(), lastError().c_str());
        return false;
    }
    m_handle = h;
    return true;
}

void shmem::NamedSemaphore::close()
{
    if (m_handle != nullptr) {
        CloseHandle(static_cast<HANDLE>(m_handle));
        m_handle = nullptr;
    }
}

void shmem::NamedSemaphore::unlink(const std::string& name)
{
    // Windows kernel objects are destroyed when the last handle is closed
    YARP_UNUSED(name);
}

bool shmem::NamedSemaphore::wait()
{
    if (m_handle == nullptr) {
        return false;
    }
    return WaitForSingleObject(static_cast<HANDLE>(m_handle), INFINITE) == WAIT_OBJECT_0;
}

bool shmem::NamedSemaphore::waitFor(double timeout)
{
    if (m_handle == nullptr) {
        return false;
    }
    const auto ms = static_cast<DWORD>(timeout > 0 ? timeout * 1000.0 : 0);
    return WaitForSingleObject(static_cast<HANDLE>(m_handle), ms) == WAIT_OBJECT_0;
}

bool shmem::NamedSemaphore::post()
{
    if (m_handle == nullptr) {
        return false;
    }
    return ReleaseSemaphore(static_cast<HANDLE>(m_handle), 1, nullptr) != 0;
}

#else // POSIX

bool shmem::NamedSemaphore::create(const std::string& name, unsigned int initial, unsigned int max)
{
    YARP_UNUSED(max);
    close();

    // Remove any stale semaphore left by a crashed process
    sem_unlink(name.c_str());

    sem_t* s = sem_open(name.c_str(), O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, initial);
    if (s == SEM_FAILED) {
        yCError(SHMEMCARRIER, "Cannot create semaphore %s (%s)", name.c_str(), lastError().c_str());
        return false;
    }
    m_handle = s;
    return true;
}

bool shmem::NamedSemaphore::open(const std::string& name)
{
    close();

    sem_t* s = sem_open(name.c_str(), 0);
    if (s == SEM_FAILED) {
        yCError(SHMEMCARRIER, "Cannot open semaphore %s (%s)", name.c_str(), lastError().c_str());
        return false;
    }
    m_handle = s;
    return true;
}

void shmem::NamedSemaphore::close()
{
    if (m_handle != nullptr) {
        sem_close(static_cast<sem_t*>(m_handle));
        m_handle = nullptr;
    }
}

void shmem::NamedSemaphore::unlink(const std::string& name)
{
    sem_unlink(name.c_str());
}

bool shmem::NamedSemaphore::wait()
{
    if (m_handle == nullptr) {
        return false;
    }
    while (sem_wait(static_cast<sem_t*>(m_handle)) != 0) {
        if (errno != EINTR) {
            return false;
        }
    }
    return true;
}

bool shmem::NamedSemaphore::waitFor(double timeout)
{
    if (m_handle == nullptr) {
        return false;
    }
    auto* s = static_cast<sem_t*>(m_handle);

#if defined(__APPLE__)
    // sem_timedwait is not available on macOS
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout);
    while (true) {
        if (sem_trywait(s) == 0) {
            return true;
        }
        if (errno != EAGAIN && errno != EINTR) {
            return false;
        }
        if (std::chrono::steady_clock::now() >= deadline) {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
#else
    // The timeout of sem_timedwait is an absolute time
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    const auto sec = static_cast<time_t>(timeout);
    ts.tv_sec += sec;
    ts.tv_nsec += static_cast<long>((timeout - static_cast<double>(sec)) * 1e9);
    if (ts.tv_nsec >= 1000000000L) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000L;
    }
    while (sem_timedwait(s, &ts) != 0) {
        if (errno != EINTR) {
            return false;
        }
    }
    return true;
#endif
}

bool shmem::NamedSemaphore::post()
{
    if (m_handle == nullptr) {
        return false;
    }
    return sem_post(static_cast<sem_t*>(m_handle)) == 0;
}

#endif


/******************************************************************************
 * Socket
 ******************************************************************************/

shmem::Socket::~Socket()
{
    close();
}

bool shmem::Socket::listen(int port)
{
    ensureSocketInit();
    close();

    native_socket_t s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == invalid_socket) {
        yCError(SHMEMCARRIER, "Cannot create socket (%s)", lastSocketError().c_str());
        return false;
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if (::bind(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        yCError(SHMEMCARRIER, "Cannot bind socket to port %d (%s)", port, lastSocketError().c_str());
        closeNativeSocket(s);
        return false;
    }

    if (::listen(s, 5) != 0) {
        yCError(SHMEMCARRIER, "Cannot listen on socket (%s)", lastSocketError().c_str());
        closeNativeSocket(s);
        return false;
    }

    m_fd = fromNative(s);
    return true;
}

bool shmem::Socket::accept(Socket& stream)
{
    stream.close();
    if (m_fd < 0) {
        return false;
    }

    native_socket_t s;
    do {
        s = ::accept(toNative(m_fd), nullptr, nullptr);
    } while (s == invalid_socket && isInterrupted());

    if (s == invalid_socket) {
        yCError(SHMEMCARRIER, "Cannot accept connection (%s)", lastSocketError().c_str());
        return false;
    }

    stream.m_fd = fromNative(s);
    return true;
}

bool shmem::Socket::connect(const std::string& host, int port)
{
    ensureSocketInit();
    close();

    addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo* res = nullptr;
    const std::string service = std::to_string(port);
    if (getaddrinfo(host.c_str(), service.c_str(), &hints, &res) != 0 || res == nullptr) {
        yCError(SHMEMCARRIER, "Cannot resolve address %s:%d", host.c_str(), port);
        return false;
    }

    native_socket_t s = invalid_socket;
    for (addrinfo* ai = res; ai != nullptr; ai = ai->ai_next) {
        s = ::socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (s == invalid_socket) {
            continue;
        }
        int ret;
        do {
            ret = ::connect(s, ai->ai_addr, static_cast<int>(ai->ai_addrlen));
        } while (ret != 0 && isInterrupted());
        if (ret == 0) {
            break;
        }
        closeNativeSocket(s);
        s = invalid_socket;
    }
    freeaddrinfo(res);

    if (s == invalid_socket) {
        yCError(SHMEMCARRIER, "Cannot connect to %s:%d (%s)", host.c_str(), port, lastSocketError().c_str());
        return false;
    }

#if defined(__APPLE__)
    int on = 1;
    setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on));
#endif

    m_fd = fromNative(s);
    return true;
}

bool shmem::Socket::sendAll(const void* data, size_t len)
{
    if (m_fd < 0) {
        return false;
    }

#if defined(MSG_NOSIGNAL)
    const int flags = MSG_NOSIGNAL;
#else
    const int flags = 0;
#endif

    const char* p = static_cast<const char*>(data);
    while (len > 0) {
        auto ret = ::send(toNative(m_fd), p, static_cast<int>(len), flags);
        if (ret < 0) {
            if (isInterrupted()) {
                continue;
            }
            return false;
        }
        if (ret == 0) {
            return false;
        }
        p += ret;
        len -= static_cast<size_t>(ret);
    }
    return true;
}

bool shmem::Socket::recvAll(void* data, size_t len)
{
    if (m_fd < 0) {
        return false;
    }

    char* p = static_cast<char*>(data);
    while (len > 0) {
        auto ret = ::recv(toNative(m_fd), p, static_cast<int>(len), 0);
        if (ret < 0) {
            if (isInterrupted()) {
                continue;
            }
            return false;
        }
        if (ret == 0) {
            return false;
        }
        p += ret;
        len -= static_cast<size_t>(ret);
    }
    return true;
}

bool shmem::Socket::isPeerClosed()
{
    if (m_fd < 0) {
        return true;
    }
    // Nothing is ever sent on this socket after the handshake
    char buf;
    auto ret = ::recv(toNative(m_fd), &buf, 1, 0);
    if (ret == 0) {
        // Orderly shutdown
        return true;
    }
    if (ret > 0) {
        return false;
    }
    // When a process dies, the connection is usually reset (e.g. on Windows
    // recv fails with WSAECONNRESET), therefore any error except "no data
    // available" means that the peer is gone
#if defined(_WIN32)
    const int err = WSAGetLastError();
    return !(err == WSAEWOULDBLOCK || err == WSAEINTR);
#else
    return !(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR);
#endif
}

bool shmem::Socket::setNonBlocking()
{
    if (m_fd < 0) {
        return false;
    }
#if defined(_WIN32)
    u_long mode = 1;
    return ioctlsocket(toNative(m_fd), FIONBIO, &mode) == 0;
#else
    int flags = fcntl(toNative(m_fd), F_GETFL, 0);
    if (flags < 0) {
        return false;
    }
    return fcntl(toNative(m_fd), F_SETFL, flags | O_NONBLOCK) == 0;
#endif
}

bool shmem::Socket::getLocalAddress(std::string& host, int& port) const
{
    if (m_fd < 0) {
        return false;
    }
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    if (getsockname(toNative(m_fd), reinterpret_cast<sockaddr*>(&addr), &len) != 0) {
        return false;
    }
    return addressToString(addr, host, port);
}

bool shmem::Socket::getRemoteAddress(std::string& host, int& port) const
{
    if (m_fd < 0) {
        return false;
    }
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    if (getpeername(toNative(m_fd), reinterpret_cast<sockaddr*>(&addr), &len) != 0) {
        return false;
    }
    return addressToString(addr, host, port);
}

void shmem::Socket::close()
{
    if (m_fd >= 0) {
        closeNativeSocket(toNative(m_fd));
        m_fd = -1;
    }
}
