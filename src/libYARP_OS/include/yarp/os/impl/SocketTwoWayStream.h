/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_SOCKETTWOWAYSTREAM_H
#define YARP_OS_IMPL_SOCKETTWOWAYSTREAM_H

#include <yarp/conf/system.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PlatformTime.h>
#include <yarp/os/impl/TcpStream.h>
#include <yarp/os/impl/TcpAcceptor.h>

#ifdef YARP_HAS_ACE // For TCP_CORK definition
# include <ace/os_include/netinet/os_tcp.h>
// In one the ACE headers there is a definition of "main" for WIN32
# ifdef main
#  undef main
# endif
#else
# include <netinet/tcp.h>
#endif

namespace yarp {
    namespace os {
        namespace impl {
            class SocketTwoWayStream;
        }
    }
}

/**
 * A stream abstraction for socket communication.  It supports TCP.
 */
class YARP_OS_impl_API yarp::os::impl::SocketTwoWayStream : public TwoWayStream,
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

    virtual ~SocketTwoWayStream()
    {
        close();
    }

    virtual InputStream& getInputStream() override
    {
        return *this;
    }

    virtual OutputStream& getOutputStream() override
    {
        return *this;
    }

    virtual const Contact& getLocalAddress() override
    {
        return localAddress;
    }

    virtual const Contact& getRemoteAddress() override
    {
        return remoteAddress;
    }

    virtual void interrupt() override
    {
        YARP_DEBUG(Logger::get(), "^^^^^^^^^^^ interrupting socket");
        if (happy) {
            happy = false;
            stream.close_reader();
            YARP_DEBUG(Logger::get(), "^^^^^^^^^^^ interrupting socket reader");
            stream.close_writer();
            YARP_DEBUG(Logger::get(), "^^^^^^^^^^^ interrupting socket writer");
            stream.close();
            YARP_DEBUG(Logger::get(), "^^^^^^^^^^^ interrupting socket fully");
        }
    }

    virtual void close() override
    {
        stream.close();
        happy = false;
    }

    using yarp::os::InputStream::read;
    virtual yarp::conf::ssize_t read(const Bytes& b) override
    {
        if (!isOk()) { return -1; }
        yarp::conf::ssize_t result;
        if (haveReadTimeout) {
            result = stream.recv_n(b.get(), b.length(), &readTimeout);
        } else {
            result = stream.recv_n(b.get(), b.length());
        }
        if (!happy) { return -1; }
        if (result<=0) {
            happy = false;
            YARP_DEBUG(Logger::get(), "bad socket read");
        }
        return result;
    }

    virtual yarp::conf::ssize_t partialRead(const Bytes& b) override
    {
        if (!isOk()) { return -1; }
        yarp::conf::ssize_t result;
        if (haveReadTimeout) {
            result = stream.recv(b.get(), b.length(), &readTimeout);
        } else {
            result = stream.recv(b.get(), b.length());
        }
        if (!happy) { return -1; }
        if (result<=0) {
            happy = false;
            YARP_DEBUG(Logger::get(), "bad socket read");
        }
        return result;
    }

    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b) override
    {
        if (!isOk()) { return; }
        yarp::conf::ssize_t result;
        if (haveWriteTimeout) {
            result = stream.send_n(b.get(), b.length(), &writeTimeout);
        } else {
            result = stream.send_n(b.get(), b.length());
        }
        if (result<0) {
            happy = false;
            YARP_DEBUG(Logger::get(), "bad socket write");
        }
    }

    virtual void flush() override
    {
#ifdef TCP_CORK
        int status = 0;
        int sizeInt = sizeof(int);
        stream.get_option(IPPROTO_TCP, TCP_CORK, &status, &sizeInt);
        if (status == 1)
        {
            // Remove CORK
            int zero = 0;
            stream.set_option(IPPROTO_TCP, TCP_CORK, &zero, sizeof(int));
            // Set CORK
            int one = 1;
            stream.set_option(IPPROTO_TCP, TCP_CORK, &one, sizeof(int));
        }
#endif
    }

    virtual bool isOk() override
    {
        return happy;
    }

    virtual void reset() override
    {
    }

    virtual void beginPacket() override
    {
#ifdef TCP_CORK
        // Set CORK
        int one = 1;
        stream.set_option(IPPROTO_TCP, TCP_CORK, &one, sizeof(int));
#endif
    }

    virtual void endPacket() override
    {
#ifdef TCP_CORK
        // Remove CORK
        int zero = 0;
        stream.set_option(IPPROTO_TCP, TCP_CORK, &zero, sizeof(int));
#endif
    }

    virtual bool setWriteTimeout(double timeout) override
    {
        if (timeout<1e-12) {
            haveWriteTimeout = false;
        } else {
            PLATFORM_TIME_SET(writeTimeout, timeout);
            haveWriteTimeout = true;
        }
        return true;
    }

    virtual bool setReadTimeout(double timeout) override
    {
        if (timeout<1e-12) {
            haveReadTimeout = false;
        } else {
            PLATFORM_TIME_SET(readTimeout, timeout);
            haveReadTimeout = true;
        }
        return true;
    }

    virtual bool setTypeOfService(int tos) override;
    virtual int getTypeOfService() override;

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

#endif // YARP_OS_IMPL_SOCKETTWOWAYSTREAM_H
