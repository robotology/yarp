/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_DGRAMTWOWAYSTREAM
#define YARP2_DGRAMTWOWAYSTREAM

#include <yarp/os/TwoWayStream.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Semaphore.h>

#include <yarp/os/impl/PlatformStdlib.h>

#ifdef YARP_HAS_ACE
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Mcast.h>
#endif

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
        dgram = YARP_NULLPTR;
        mgram = YARP_NULLPTR;
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

    virtual bool open(const Contact& remote);

    virtual bool open(const Contact& local, const Contact& remote);

    virtual bool openMcast(const Contact& group,
                           const Contact& ipLocal);

#ifdef YARP_HAS_ACE
    virtual int restrictMcast(ACE_SOCK_Dgram_Mcast * dmcast,
                              const Contact& group,
                              const Contact& ipLocal,
                              bool add);
#endif

    virtual bool join(const Contact& group, bool sender,
                      const Contact& ipLocal);

    virtual bool join(const Contact& group, bool sender) {
        return join(group,sender,Contact());
    }

    virtual ~DgramTwoWayStream();

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

    virtual void interrupt();

    virtual void close() {
        closeMain();
    }

    virtual void closeMain();

    using yarp::os::InputStream::read;
    virtual YARP_SSIZE_T read(const yarp::os::Bytes& b);

    using yarp::os::OutputStream::write;
    virtual void write(const yarp::os::Bytes& b);

    virtual void flush();

    virtual bool isOk();

    virtual void reset();

    virtual void beginPacket();

    virtual void endPacket();

    yarp::os::Bytes getMonitor();

    virtual bool setTypeOfService(int tos);

    virtual int getTypeOfService();

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
#ifdef YARP_HAS_ACE
    ACE_SOCK_Dgram *dgram;
    ACE_SOCK_Dgram_Mcast *mgram;
    ACE_INET_Addr localHandle, remoteHandle;
#else
    int dgram_sockfd;
    void *dgram;
    void *mgram;
    int localHandle, remoteHandle;
#endif
    Contact localAddress, remoteAddress, restrictInterfaceIp;
    yarp::os::ManagedBytes readBuffer, writeBuffer;
    yarp::os::Semaphore mutex;
    YARP_SSIZE_T readAt, readAvail, writeAvail;
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

