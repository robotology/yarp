// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_SOCKETTWOWAYSTREAM
#define YARP2_SOCKETTWOWAYSTREAM

#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/conf/system.h>

#  include <yarp/os/impl/PlatformTime.h>

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
class yarp::os::impl::SocketTwoWayStream : public TwoWayStream,
            InputStream, OutputStream {
public:
    SocketTwoWayStream() :
            haveWriteTimeout(false),
            haveReadTimeout(false),
            happy(false)
    {
    }

    int open(const Contact& address);

    int open(ACE_SOCK_Acceptor& acceptor);

    virtual ~SocketTwoWayStream() {
        close();
    }

    virtual InputStream& getInputStream() {
        return *this;
    }

    virtual OutputStream& getOutputStream() {
        return *this;
    }

    virtual const Contact& getLocalAddress() {
        return localAddress;
    }

    virtual const Contact& getRemoteAddress() {
        return remoteAddress;
    }

    virtual void interrupt() {
        YARP_DEBUG(Logger::get(),"^^^^^^^^^^^ interrupting socket");
        if (happy) {
            happy = false;
            stream.close_reader();
            YARP_DEBUG(Logger::get(),"^^^^^^^^^^^ interrupting socket reader");
            stream.close_writer();
            YARP_DEBUG(Logger::get(),"^^^^^^^^^^^ interrupting socket writer");
            stream.close();
            YARP_DEBUG(Logger::get(),"^^^^^^^^^^^ interrupting socket fully");
        }
        //stream.close_writer();
        //stream.close();
    }

    virtual void close() {
        stream.close();
        happy = false;
    }

    using yarp::os::InputStream::read;
    virtual YARP_SSIZE_T read(const Bytes& b) {
        if (!isOk()) { return -1; }
        YARP_SSIZE_T result;
        if (haveReadTimeout) {
            result = stream.recv_n(b.get(),b.length(),&readTimeout);
        } else {
            result = stream.recv_n(b.get(),b.length());
        }
        if (!happy) { return -1; }
        if (result<=0) {
            happy = false;
            YARP_DEBUG(Logger::get(),"bad socket read");
        }
        return result;
    }

    virtual YARP_SSIZE_T partialRead(const Bytes& b) {
        if (!isOk()) { return -1; }
        YARP_SSIZE_T result;
        if (haveReadTimeout) {
            result = stream.recv(b.get(),b.length(),&readTimeout);
        } else {
            result = stream.recv(b.get(),b.length());
        }
        if (!happy) { return -1; }
        if (result<=0) {
            happy = false;
            YARP_DEBUG(Logger::get(),"bad socket read");
        }
        return result;
    }

    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b) {
        if (!isOk()) { return; }
        YARP_SSIZE_T result;
        if (haveWriteTimeout) {
            result = stream.send_n(b.get(),b.length(),&writeTimeout);
        } else {
            result = stream.send_n(b.get(),b.length());
        }
        if (result<0) {
            happy = false;
            YARP_DEBUG(Logger::get(),"bad socket write");
        }
    }

    virtual void flush() {
        //stream.flush();
    }

    virtual bool isOk() {
        return happy;
    }

    virtual void reset() {
    }

    virtual void beginPacket() {
    }

    virtual void endPacket() {
    }

    virtual bool setWriteTimeout(double timeout) {
        if (timeout<1e-12) {
            haveWriteTimeout = false;
        } else {
            PLATFORM_TIME_SET(writeTimeout,timeout);
            haveWriteTimeout = true;
        }
        return true;
    }

    virtual bool setReadTimeout(double timeout) {
        if (timeout<1e-12) {
            haveReadTimeout = false;
        } else {
            PLATFORM_TIME_SET(readTimeout,timeout);
            haveReadTimeout = true;
        }
        return true;
    }

    virtual bool setTypeOfService(int tos);    
    virtual int getTypeOfService();

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

#endif
