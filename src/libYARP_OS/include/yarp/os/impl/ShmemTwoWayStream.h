// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_SHMEMTWOWAYSTREAM
#define YARP2_SHMEMTWOWAYSTREAM

#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/Logger.h>

#include <ace/config.h>
#include <ace/MEM_Acceptor.h>
#include <ace/MEM_Connector.h>
#include <ace/Log_Msg.h>

namespace yarp {
    namespace os {
        namespace impl {
            class ShmemTwoWayStream;
        }
    }
}

/**
 * A stream abstraction for shared memory / socket hybrid communication.
 */
class yarp::os::impl::ShmemTwoWayStream : public TwoWayStream,
            InputStream, OutputStream {
public:
    ShmemTwoWayStream() {
        happy = false;
        currentLength = 0;
    }

    int open(const Address& address, bool sender);

    int accept();

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

    virtual int read(const Bytes& b);

    virtual void write(const Bytes& b);

    virtual void flush();

    virtual bool isOk() {
        return happy;
    }

    virtual void reset();

    virtual void beginPacket();

    virtual void endPacket();

private:
    ACE_MEM_Stream stream;
    Address localAddress, remoteAddress;
    ACE_MEM_Acceptor acceptor;
    bool happy;
    int currentLength;
    void updateAddresses();

    void reconnect();
};

#endif

