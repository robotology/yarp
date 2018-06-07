/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_DGRAMTWOWAYSTREAM_H
#define YARP_OS_IMPL_DGRAMTWOWAYSTREAM_H

#include <yarp/os/TwoWayStream.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Mutex.h>

#include <cstdlib>

#ifdef YARP_HAS_ACE
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Mcast.h>
// In one the ACE headers there is a definition of "main" for WIN32
# ifdef main
#  undef main
# endif
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
class YARP_OS_impl_API yarp::os::impl::DgramTwoWayStream : public TwoWayStream, public InputStream, public OutputStream
{

public:
    DgramTwoWayStream() :
                          closed(false), interrupting(false), reader(false),
#ifndef YARP_HAS_ACE
                          dgram_sockfd(-1),
#endif
                          dgram(nullptr), mgram(nullptr),
                          mutex(), readAt(0), readAvail(0),
                          writeAvail(0), pct(0), happy(true),
                          bufferAlertNeeded(false), bufferAlerted(false),
                          multiMode(false), errCount(0), lastReportTime(0)
    {
    }

    virtual bool openMonitor(int readSize=0, int writeSize=0)
    {
        allocate(readSize, writeSize);
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

    virtual bool join(const Contact& group, bool sender)
    {
        return join(group, sender, Contact());
    }

    virtual ~DgramTwoWayStream();

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

    virtual void interrupt() override;

    virtual void close() override
    {
        closeMain();
    }

    virtual void closeMain();

    using yarp::os::InputStream::read;
    virtual yarp::conf::ssize_t read(const yarp::os::Bytes& b) override;

    using yarp::os::OutputStream::write;
    virtual void write(const yarp::os::Bytes& b) override;

    virtual void flush() override;

    virtual bool isOk() override;

    virtual void reset() override;

    virtual void beginPacket() override;

    virtual void endPacket() override;

    yarp::os::Bytes getMonitor();

    virtual bool setTypeOfService(int tos) override;

    virtual int getTypeOfService() override;

    void setMonitor(const yarp::os::Bytes& data)
    {
        monitor = yarp::os::ManagedBytes(data, false);
        monitor.copy();
    }

    void removeMonitor();

    virtual void onMonitorInput() {}

    virtual void onMonitorOutput() {}

private:
    yarp::os::ManagedBytes monitor;
    bool closed, interrupting, reader;
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
    yarp::os::Mutex mutex;
    yarp::conf::ssize_t readAt, readAvail, writeAvail;
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

#endif // YARP_OS_IMPL_DGRAMTWOWAYSTREAM_H
