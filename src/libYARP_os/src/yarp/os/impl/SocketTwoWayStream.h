/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_SOCKETTWOWAYSTREAM_H
#define YARP_OS_IMPL_SOCKETTWOWAYSTREAM_H

#include <yarp/conf/system.h>

#include <yarp/os/Bytes.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/PlatformTime.h>
#include <yarp/os/impl/TcpAcceptor.h>
#include <yarp/os/impl/TcpStream.h>

#ifdef YARP_HAS_ACE // For TCP_CORK definition
#    include <ace/os_include/netinet/os_tcp.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#else
#    include <netinet/tcp.h>
#endif

YARP_DECLARE_LOG_COMPONENT(SOCKETTWOWAYSTREAM)

namespace yarp {
namespace os {
namespace impl {

/**
 * A stream abstraction for socket communication.  It supports TCP.
 */
class YARP_os_impl_API SocketTwoWayStream :
        public TwoWayStream,
        public InputStream,
        public OutputStream
{
public:
    SocketTwoWayStream() :
            haveWriteTimeout(false),
            haveReadTimeout(false),
            happy(false)
    {
    }

    int open(const Contact& address);

    int open(yarp::os::impl::TcpAcceptor& acceptor);

    ~SocketTwoWayStream() override
    {
        close();
    }

    InputStream& getInputStream() override
    {
        return *this;
    }

    OutputStream& getOutputStream() override
    {
        return *this;
    }

    const Contact& getLocalAddress() const override
    {
        return localAddress;
    }

    const Contact& getRemoteAddress() const override
    {
        return remoteAddress;
    }

    void interrupt() override
    {
        yCDebug(SOCKETTWOWAYSTREAM, "Interrupting socket");
        if (happy) {
            happy = false;
            stream.close_reader();
            yCDebug(SOCKETTWOWAYSTREAM, "Interrupting socket reader");
            stream.close_writer();
            yCDebug(SOCKETTWOWAYSTREAM, "Interrupting socket writer");
            stream.close();
            yCDebug(SOCKETTWOWAYSTREAM, "Interrupting socket fully");
        }
    }

    void close() override
    {
        stream.close();
        happy = false;
    }

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(Bytes& b) override
    {
        if (!isOk()) {
            return -1;
        }
        yarp::conf::ssize_t result;
        if (haveReadTimeout) {
            result = stream.recv_n(b.get(), b.length(), &readTimeout);
        } else {
            result = stream.recv_n(b.get(), b.length());
        }
        if (!happy) {
            return -1;
        }
        if (result <= 0) {
            happy = false;
            yCDebug(SOCKETTWOWAYSTREAM, "bad socket read");
        }
        return result;
    }

    yarp::conf::ssize_t partialRead(Bytes& b) override
    {
        if (!isOk()) {
            return -1;
        }
        yarp::conf::ssize_t result;
        if (haveReadTimeout) {
            result = stream.recv(b.get(), b.length(), &readTimeout);
        } else {
            result = stream.recv(b.get(), b.length());
        }
        if (!happy) {
            return -1;
        }
        if (result <= 0) {
            happy = false;
            yCDebug(SOCKETTWOWAYSTREAM, "bad socket read");
        }
        return result;
    }

    using yarp::os::OutputStream::write;
    void write(const Bytes& b) override
    {
        if (!isOk()) {
            return;
        }
        yarp::conf::ssize_t result;
        if (haveWriteTimeout) {
            result = stream.send_n(b.get(), b.length(), &writeTimeout);
        } else {
            result = stream.send_n(b.get(), b.length());
        }
        if (result < 0) {
            happy = false;
            yCDebug(SOCKETTWOWAYSTREAM, "bad socket write");
        }
    }

    void flush() override
    {
#ifdef TCP_CORK
        int status = 0;
        int sizeInt = sizeof(int);
        stream.get_option(IPPROTO_TCP, TCP_CORK, &status, &sizeInt);
        if (status == 1) {
            // Remove CORK
            int zero = 0;
            stream.set_option(IPPROTO_TCP, TCP_CORK, &zero, sizeof(int));
            // Set CORK
            int one = 1;
            stream.set_option(IPPROTO_TCP, TCP_CORK, &one, sizeof(int));
        }
#endif
    }

    bool isOk() const override
    {
        return happy;
    }

    void reset() override
    {
    }

    void beginPacket() override
    {
#ifdef TCP_CORK
        // Set CORK
        int one = 1;
        stream.set_option(IPPROTO_TCP, TCP_CORK, &one, sizeof(int));
#endif
    }

    void endPacket() override
    {
#ifdef TCP_CORK
        // Remove CORK
        int zero = 0;
        stream.set_option(IPPROTO_TCP, TCP_CORK, &zero, sizeof(int));
#endif
    }

    bool setWriteTimeout(double timeout) override
    {
        if (timeout < 1e-12) {
            haveWriteTimeout = false;
        } else {
            PLATFORM_TIME_SET(writeTimeout, timeout);
            haveWriteTimeout = true;
        }
        return true;
    }

    bool setReadTimeout(double timeout) override
    {
        if (timeout < 1e-12) {
            haveReadTimeout = false;
        } else {
            PLATFORM_TIME_SET(readTimeout, timeout);
            haveReadTimeout = true;
        }
        return true;
    }

    bool setTypeOfService(int tos) override;
    int getTypeOfService() override;

private:
    yarp::os::impl::TcpStream stream;
    bool haveWriteTimeout;
    bool haveReadTimeout;
    YARP_timeval writeTimeout;
    YARP_timeval readTimeout;
    Contact localAddress, remoteAddress;
    bool happy;
    void updateAddresses();
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_SOCKETTWOWAYSTREAM_H
