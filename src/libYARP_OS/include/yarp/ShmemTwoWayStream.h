// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_SHMEMTWOWAYSTREAM_
#define _YARP2_SHMEMTWOWAYSTREAM_

#include <yarp/TwoWayStream.h>
#include <yarp/IOException.h>
#include <yarp/Logger.h>

#include <ace/config.h>
#include <ace/MEM_Acceptor.h>
#include <ace/MEM_Connector.h>
#include <ace/Log_Msg.h>

namespace yarp {
    class ShmemTwoWayStream;
}

/**
 * A stream abstraction for shared memory / socket hybrid communication.
 */
class yarp::ShmemTwoWayStream : public TwoWayStream, 
            InputStream, OutputStream {
public:
    ShmemTwoWayStream() {
        happy = false;
    }

    int open(const Address& address, bool sender);

    //void open(ACE_MEM_Acceptor& acceptor);

    virtual ~ShmemTwoWayStream() {
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
        stream.close_reader();
        stream.close_writer();
        happy = false;
    }

    virtual void close() {
        stream.close();
        happy = false;
    }

    virtual int read(const Bytes& b) {
        int result = stream.recv_n(b.get(),b.length());
        if (result<=0) {
            happy = false;
            YARP_DEBUG(Logger::get(),"bad socket read");
        }
        return result;
    }

    virtual void write(const Bytes& b) {
        int result = stream.send_n(b.get(),b.length());
        if (result<0) {
            happy = false;
            YARP_DEBUG(Logger::get(),"bad socket write");
        }
    }

    virtual void flush();

    virtual bool isOk() {
        return happy;
    }
  
    virtual void reset() {
    }

    virtual void beginPacket() { }

    virtual void endPacket() { }

private:
    ACE_MEM_Stream stream;
    Address localAddress, remoteAddress;
    bool happy;
    void updateAddresses();
};

#endif

