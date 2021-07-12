/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/conf/system.h>

#include <yarp/os/LogStream.h>
#include <yarp/os/NetType.h>
#include <yarp/os/SystemClock.h>

#include "UnixSockTwoWayStream.h"
#include "UnixSocketLogComponent.h"
#include <cerrno>
#include <cstring>
#include <fcntl.h> /* For O_* constants */
#include <sys/socket.h>
#include <sys/stat.h> /* For mode constants */
#include <sys/un.h>
#include <unistd.h>

using namespace yarp::os;

UnixSockTwoWayStream::UnixSockTwoWayStream(const std::string& _socketPath) :
        socketPath(_socketPath)
{
}

bool UnixSockTwoWayStream::open(bool sender)
{
    openedAsReader = !sender;
    struct sockaddr_un addr;
    if ((reader_fd = ::socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        yCError(UNIXSOCK_CARRIER, "%d, %s", errno, strerror(errno));
        return false;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    if (socketPath.empty()) {
        *addr.sun_path = '\0';
        strncpy(addr.sun_path + 1, socketPath.c_str() + 1, sizeof(addr.sun_path) - 2);
    } else {
        strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);
        if (!sender) {
            ::unlink(socketPath.c_str());
        }
    }

    if (sender) {
        size_t attempts = 0;
        // try connection 5 times, waiting that the receiver bind the socket
        while (attempts < maxAttempts) {
            int result = ::connect(reader_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
            if (result == 0) {
                break;
            }
            yarp::os::SystemClock::delaySystem(delayBetweenAttempts);
            attempts++;
        }

        if (attempts >= maxAttempts) {
            yCError(UNIXSOCK_CARRIER, "connect() error, I tried %zu times: %d, %s", maxAttempts, errno, strerror(errno));
            return false;
        }
    } else {
        if (::bind(reader_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1) {
            yCError(UNIXSOCK_CARRIER, "bind() error: %d, %s", errno, strerror(errno));
            return false;
        }

        // the socket will listen only 1 client
        if (::listen(reader_fd, 2) == -1) {
            yCError(UNIXSOCK_CARRIER, "listen() error: %d, %s", errno, strerror(errno));
            return false;
        }
        struct sockaddr_un remote;
        uint lenRemote = sizeof(remote);

        if ((sender_fd = ::accept(reader_fd, reinterpret_cast<struct sockaddr*>(&remote), &lenRemote)) == -1) {
            yCError(UNIXSOCK_CARRIER, "accept() error: %d, %s", errno, strerror(errno));
            return false;
        }
    }

    return true;
}

UnixSockTwoWayStream::~UnixSockTwoWayStream()
{
    close();
}

InputStream& UnixSockTwoWayStream::getInputStream()
{
    return *this;
}

OutputStream& UnixSockTwoWayStream::getOutputStream()
{
    return *this;
}

const Contact& UnixSockTwoWayStream::getLocalAddress() const
{
    return localAddress;
}

const Contact& UnixSockTwoWayStream::getRemoteAddress() const
{
    return remoteAddress;
}

void UnixSockTwoWayStream::setLocalAddress(Contact& _localAddress)
{
    localAddress = _localAddress;
}

void UnixSockTwoWayStream::setRemoteAddress(Contact& _remoteAddress)
{
    remoteAddress = _remoteAddress;
}

void UnixSockTwoWayStream::interrupt()
{
    yCDebug(UNIXSOCK_CARRIER, "Interrupting socket");
    close();
}

void UnixSockTwoWayStream::close()
{
    // If the connect descriptor is valid close socket
    // and free the memory dedicated.
    // socket closure
    if (sender_fd > 0) {
        ::shutdown(sender_fd, SHUT_RDWR);
        ::close(sender_fd);
        sender_fd = -1;
    }

    if (reader_fd > 0) {
        ::shutdown(reader_fd, SHUT_RDWR);
        ::close(reader_fd);
        reader_fd = -1;
    }

    ::unlink(socketPath.c_str());
    happy = false;
}

yarp::conf::ssize_t UnixSockTwoWayStream::read(Bytes& b)
{
    if (closed || !happy) {
        return -1;
    }
    int result;
    result = ::read(openedAsReader ? sender_fd : reader_fd, b.get(), b.length());
    if (closed || result == 0) {
        happy = false;
        return -1;
    }
    if (result < 0) {
        yCError(UNIXSOCK_CARRIER, "read() error: %d, %s", errno, strerror(errno));
        return -1;
    }
    return result;
}

void UnixSockTwoWayStream::write(const Bytes& b)
{
    if (reader_fd < 0) {
        close();
        return;
    }
    int writtenMem = ::write(openedAsReader ? sender_fd : reader_fd, b.get(), b.length());
    if (writtenMem < 0) {
        yCError(UNIXSOCK_CARRIER, "write() error: %d, %s", errno, strerror(errno));
        if (errno != ETIMEDOUT) {
            close();
        }
        return;
    }
}

bool UnixSockTwoWayStream::isOk() const
{
    return happy;
}

void UnixSockTwoWayStream::reset()
{
}

void UnixSockTwoWayStream::beginPacket()
{
}

void UnixSockTwoWayStream::endPacket()
{
}
