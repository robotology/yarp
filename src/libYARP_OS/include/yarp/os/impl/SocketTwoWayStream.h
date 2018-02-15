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

#ifdef YARP_HAS_ACE
#  include <ace/config.h>
#  include <ace/SOCK_Acceptor.h>
#  include <ace/SOCK_Connector.h>
#  include <ace/SOCK_Stream.h>
#  include <ace/Log_Msg.h>
#  include <ace/Time_Value.h>
#else
#  include <yarp/os/impl/TcpStream.h>
#  include <yarp/os/impl/TcpAcceptor.h>
#  define ACE_SOCK_Acceptor TcpAcceptor
#  define ACE_SOCK_Stream TcpStream
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

    int open(ACE_SOCK_Acceptor& acceptor);

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
    virtual YARP_SSIZE_T read(const Bytes& b) override
    {
        if (!isOk()) { return -1; }
        YARP_SSIZE_T result;
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

    virtual YARP_SSIZE_T partialRead(const Bytes& b) override
    {
        if (!isOk()) { return -1; }
        YARP_SSIZE_T result;
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
        YARP_SSIZE_T result;
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
        //stream.flush();
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
    }

    virtual void endPacket() override
    {
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
    ACE_SOCK_Stream stream;
    bool haveWriteTimeout;
    bool haveReadTimeout;
    ACE_Time_Value writeTimeout;
    ACE_Time_Value readTimeout;
    Contact localAddress, remoteAddress;
    bool happy;
    void updateAddresses();
};

#endif // YARP_OS_IMPL_SOCKETTWOWAYSTREAM_H
