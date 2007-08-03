// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_DGRAMTWOWAYSTREAM_
#define _YARP2_DGRAMTWOWAYSTREAM_

#include <yarp/TwoWayStream.h>
#include <yarp/IOException.h>
#include <yarp/ManagedBytes.h>
#include <yarp/os/Semaphore.h>

#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Mcast.h>

namespace yarp {
    class DgramTwoWayStream;
}

/**
 * A stream abstraction for datagram communication.  It supports UDP and
 * MCAST.  This class is not concerned with making the stream reliable.
 */
class yarp::DgramTwoWayStream : public TwoWayStream, public InputStream, public OutputStream {

public:
    DgramTwoWayStream() : mutex(1) {
        interrupting = false;
        closed = false;
        reader = false;
        writer = false;
        dgram = NULL;
        mgram = NULL;
        happy = true;
        bufferAlerted = bufferAlertNeeded = false;
        multiMode = false;
    }

    virtual void open(const Address& remote);

    virtual void open(const Address& local, const Address& remote);

    virtual void openMcast(const Address& group,
                           const Address& ipLocal);

    virtual int restrictMcast(ACE_SOCK_Dgram_Mcast * dmcast,
                              const Address& group,
                              const Address& ipLocal,
                              bool add);

    virtual void join(const Address& group, bool sender,
                      const Address& ipLocal);

    virtual void join(const Address& group, bool sender) {
        join(group,sender,Address());
    }

    virtual ~DgramTwoWayStream();

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

    virtual void interrupt();

    virtual void close() {
        closeMain();
    }

    virtual void closeMain();

    virtual int read(const Bytes& b);

    virtual void write(const Bytes& b);

    virtual void flush();

    virtual bool isOk();

    virtual void reset();

    virtual void beginPacket();

    virtual void endPacket();

private:

    bool closed, interrupting, reader, writer;
    ACE_SOCK_Dgram *dgram;
    ACE_SOCK_Dgram_Mcast *mgram;
    ACE_INET_Addr localHandle, remoteHandle;
    Address localAddress, remoteAddress, restrictInterfaceIp;
    ManagedBytes readBuffer, writeBuffer;
    yarp::os::Semaphore mutex;
    int readAt, readAvail, writeAvail, pct;
    bool happy;
    bool bufferAlertNeeded;
    bool bufferAlerted;
    bool multiMode;

    void allocate();

    void configureSystemBuffers();
};

#endif

