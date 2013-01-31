// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_DGRAMTWOWAYSTREAM_
#define _YARP2_DGRAMTWOWAYSTREAM_

#include <yarp/os/impl/TwoWayStream.h>
#include <yarp/os/impl/IOException.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Semaphore.h>

#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Mcast.h>

namespace yarp {
    namespace os {
        namespace impl {
            class DgramTwoWayStream;
        }
    }
}

/**
 * A stream abstraction for datagram communication.  It supports UDP and
 * MCAST.  This class is not concerned with making the stream reliable.
 */
class YARP_OS_impl_API yarp::os::impl::DgramTwoWayStream : public TwoWayStream, public InputStream, public OutputStream {

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
        errCount = 0;
        lastReportTime = 0;
    }

    virtual bool openMonitor(int readSize=0, int writeSize=0) {
        allocate(readSize,writeSize);
        return true;
    }

    virtual bool open(const Address& remote);

    virtual bool open(const Address& local, const Address& remote);

    virtual bool openMcast(const Address& group,
                           const Address& ipLocal);

    virtual int restrictMcast(ACE_SOCK_Dgram_Mcast * dmcast,
                              const Address& group,
                              const Address& ipLocal,
                              bool add);

    virtual bool join(const Address& group, bool sender,
                      const Address& ipLocal);

    virtual bool join(const Address& group, bool sender) {
        return join(group,sender,Address());
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

    virtual ssize_t read(const yarp::os::Bytes& b);

    virtual void write(const yarp::os::Bytes& b);

    virtual void flush();

    virtual bool isOk();

    virtual void reset();

    virtual void beginPacket();

    virtual void endPacket();

    yarp::os::Bytes getMonitor();

    void setMonitor(const yarp::os::Bytes& data) {
        monitor = yarp::os::ManagedBytes(data,false);
        monitor.copy();
    }

    void removeMonitor();

    virtual void onMonitorInput() {}

    virtual void onMonitorOutput() {}

private:
    yarp::os::ManagedBytes monitor;
    bool closed, interrupting, reader, writer;
    ACE_SOCK_Dgram *dgram;
    ACE_SOCK_Dgram_Mcast *mgram;
    ACE_INET_Addr localHandle, remoteHandle;
    Address localAddress, remoteAddress, restrictInterfaceIp;
    yarp::os::ManagedBytes readBuffer, writeBuffer;
    yarp::os::Semaphore mutex;
    ssize_t readAt, readAvail, writeAvail;
    int pct;
    bool happy;
    bool bufferAlertNeeded;
    bool bufferAlerted;
    bool multiMode;
    int errCount;
    double lastReportTime;

    void allocate(int readSize=0, int writeSize=0);

    void configureSystemBuffers();
};

#endif

