/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_DGRAMTWOWAYSTREAM_H
#define YARP_OS_IMPL_DGRAMTWOWAYSTREAM_H

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/TwoWayStream.h>

#include <cstdlib>
#include <mutex>

#ifdef YARP_HAS_ACE
#    include <ace/SOCK_Dgram.h>
#    include <ace/SOCK_Dgram_Mcast.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#endif

namespace yarp {
namespace os {
namespace impl {

/**
 * A stream abstraction for datagram communication.  It supports UDP and
 * MCAST.  This class is not concerned with making the stream reliable.
 */
class YARP_os_impl_API DgramTwoWayStream :
        public TwoWayStream,
        public InputStream,
        public OutputStream
{

public:
    DgramTwoWayStream() :
            closed(false),
            interrupting(false),
            reader(false),
#ifndef YARP_HAS_ACE
            dgram_sockfd(-1),
#endif
            dgram(nullptr),
            mgram(nullptr),
            mutex(),
            readAt(0),
            readAvail(0),
            writeAvail(0),
            pct(0),
            happy(true),
            bufferAlertNeeded(false),
            bufferAlerted(false),
            multiMode(false),
            errCount(0),
            lastReportTime(0)
    {
    }

    virtual bool openMonitor(int readSize = 0, int writeSize = 0)
    {
        allocate(readSize, writeSize);
        return true;
    }

    virtual bool open(const Contact& remote);

    virtual bool open(const Contact& local, const Contact& remote);

    virtual bool openMcast(const Contact& group,
                           const Contact& ipLocal);

#ifdef YARP_HAS_ACE
    virtual int restrictMcast(ACE_SOCK_Dgram_Mcast* dmcast,
                              const Contact& group,
                              const Contact& ipLocal,
                              bool add);
#endif

    virtual bool join(const Contact& group, bool sender, const Contact& ipLocal);

    virtual bool join(const Contact& group, bool sender)
    {
        return join(group, sender, Contact());
    }

    virtual ~DgramTwoWayStream();

    InputStream& getInputStream() override
    {
        return *this;
    }

    OutputStream& getOutputStream() override
    {
        return *this;
    }

    const Contact& getLocalAddress() const override
    {
        return localAddress;
    }

    const Contact& getRemoteAddress() const override
    {
        return remoteAddress;
    }

    void interrupt() override;

    void close() override
    {
        closeMain();
    }

    virtual void closeMain();

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& b) override;

    using yarp::os::OutputStream::write;
    void write(const yarp::os::Bytes& b) override;

    void flush() override;

    bool isOk() const override;

    void reset() override;

    void beginPacket() override;

    void endPacket() override;

    yarp::os::Bytes getMonitor();

    bool setTypeOfService(int tos) override;

    int getTypeOfService() override;

    void setMonitor(const yarp::os::Bytes& data)
    {
        monitor = yarp::os::ManagedBytes(data, false);
        monitor.copy();
    }

    void removeMonitor();

    virtual void onMonitorInput()
    {
    }

    virtual void onMonitorOutput()
    {
    }

private:
    yarp::os::ManagedBytes monitor;
    bool closed, interrupting, reader;
#ifdef YARP_HAS_ACE
    ACE_SOCK_Dgram* dgram;
    ACE_SOCK_Dgram_Mcast* mgram;
    ACE_INET_Addr localHandle, remoteHandle;
#else
    int dgram_sockfd;
    void* dgram;
    void* mgram;
    int localHandle, remoteHandle;
#endif
    Contact localAddress, remoteAddress, restrictInterfaceIp;
    yarp::os::ManagedBytes readBuffer, writeBuffer;
    std::mutex mutex;
    yarp::conf::ssize_t readAt, readAvail, writeAvail;
    int pct;
    bool happy;
    bool bufferAlertNeeded;
    bool bufferAlerted;
    bool multiMode;
    int errCount;
    double lastReportTime;

    void allocate(int readSize = 0, int writeSize = 0);

    void configureSystemBuffers();
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_DGRAMTWOWAYSTREAM_H
