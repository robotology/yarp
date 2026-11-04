/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SHMEM_SHMEMPLATFORM_H
#define YARP_SHMEM_SHMEMPLATFORM_H

#include <cstddef>
#include <cstdint>
#include <string>

/*
 * Minimal portable wrappers (Win32 and POSIX) around the operating system
 * primitives used by the shmem carrier: named shared memory segments, named
 * inter-process semaphores and TCP sockets.
 */
namespace shmem {

/**
 * Build the name of a named inter-process object.
 *
 * @param kind the kind of object (e.g. "data", "mtx", "sem")
 * @param port the port used as a key for the connection
 * @param index an optional index (ignored if negative)
 */
std::string objectName(const char* kind, int port, int index = -1);

/**
 * A named shared memory segment.
 */
class SharedMemory
{
public:
    SharedMemory() = default;
    ~SharedMemory();

    SharedMemory(const SharedMemory&) = delete;
    SharedMemory& operator=(const SharedMemory&) = delete;

    /// Create a new segment (removing any stale segment with the same name)
    bool create(const std::string& name, size_t size);
    /// Open an existing segment
    bool open(const std::string& name);
    /// Unmap the segment
    void close();
    /// Remove the name of the segment (no-op on Windows)
    static void unlink(const std::string& name);

    void* data() const { return m_data; }

private:
    void* m_handle {nullptr};
    void* m_data {nullptr};
    size_t m_size {0};
};

/**
 * A named inter-process semaphore.
 *
 * Unlike a process mutex, it can be released by any thread of any process.
 */
class NamedSemaphore
{
public:
    NamedSemaphore() = default;
    ~NamedSemaphore();

    NamedSemaphore(const NamedSemaphore&) = delete;
    NamedSemaphore& operator=(const NamedSemaphore&) = delete;

    /// Create a new semaphore (removing any stale semaphore with the same name)
    bool create(const std::string& name, unsigned int initial, unsigned int max);
    /// Open an existing semaphore
    bool open(const std::string& name);
    void close();
    /// Remove the name of the semaphore (no-op on Windows)
    static void unlink(const std::string& name);

    /// Wait until the semaphore is available
    bool wait();
    /// Wait at most timeout seconds. Returns true if the semaphore was acquired
    bool waitFor(double timeout);
    bool post();

private:
    void* m_handle {nullptr};
};

/**
 * A minimal IPv4 TCP socket.
 */
class Socket
{
public:
    Socket() = default;
    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    /// Listen on all the interfaces (port 0 means any free port)
    bool listen(int port);
    /// Accept a connection on a listening socket
    bool accept(Socket& stream);
    bool connect(const std::string& host, int port);

    /// Send exactly len bytes (blocking socket)
    bool sendAll(const void* data, size_t len);
    /// Receive exactly len bytes (blocking socket)
    bool recvAll(void* data, size_t len);
    /// Check, on a non-blocking socket, whether the connection was closed or
    /// reset by the peer (e.g. because the peer process died)
    bool isPeerClosed();

    bool setNonBlocking();

    bool getLocalAddress(std::string& host, int& port) const;
    bool getRemoteAddress(std::string& host, int& port) const;

    void close();

private:
    std::intptr_t m_fd {-1};
};

} // namespace shmem

#endif // YARP_SHMEM_SHMEMPLATFORM_H
