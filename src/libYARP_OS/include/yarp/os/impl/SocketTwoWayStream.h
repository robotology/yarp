// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_SOCKETTWOWAYSTREAM_
#define _YARP2_SOCKETTWOWAYSTREAM_

#include <yarp/TwoWayStream.h>
#include <yarp/IOException.h>
#include <yarp/Logger.h>

#include <ace/config.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>

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
    SocketTwoWayStream() {
        happy = false;
    }

    int open(const Address& address);

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

    virtual const Address& getLocalAddress() {
        return localAddress;
    }

    virtual const Address& getRemoteAddress() {
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
        }
        //stream.close_writer();
        //stream.close();
    }

    virtual void close() {
        stream.close();
        happy = false;
    }

    virtual int read(const Bytes& b) {
        if (!isOk()) { return -1; }
        //ACE_OS::printf("STWS::read pre \n");
        //YARP_DEBUG(Logger::get(),"^^^^^^^^^^^ recv");
        int result = stream.recv_n(b.get(),b.length());
        //YARP_DEBUG(Logger::get(),"^^^^^^^^^^^ recv done");
        if (!happy) { return -1; }
        //ACE_OS::printf("socket read %d\n", result);
        //ACE_OS::printf("STWS::read post \n");
        if (result<=0) {
            happy = false;
            YARP_DEBUG(Logger::get(),"bad socket read");
            //throw IOException("input socket died");
        }
        return result;
    }

    virtual void write(const Bytes& b) {
        if (!isOk()) { return; }
        int result = stream.send_n(b.get(),b.length());
        //ACE_OS::printf("socket write %d\n", result);
        if (result<0) {
            happy = false;
            YARP_DEBUG(Logger::get(),"bad socket write");
            //throw IOException("output socket died");
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

    virtual void beginPacket() { }

    virtual void endPacket() { }

private:
    ACE_SOCK_Stream stream;
    Address localAddress, remoteAddress;
    bool happy;
    void updateAddresses();
};

#endif

