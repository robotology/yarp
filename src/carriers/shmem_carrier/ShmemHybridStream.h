/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_SHMEMHYBRIDSTREAM_H
#define YARP_OS_IMPL_SHMEMHYBRIDSTREAM_H

#include <yarp/os/InputStream.h>
#include <yarp/os/NetType.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/Logger.h>

#include "ShmemInputStream.h"
#include "ShmemOutputStream.h"


/**
 * A stream abstraction for shared memory communication.
 */
class ShmemHybridStream : public yarp::os::TwoWayStream,
                          public yarp::os::InputStream,
                          public yarp::os::OutputStream
{
public:
    ShmemHybridStream();
    virtual ~ShmemHybridStream();

    int open(const yarp::os::Contact& yarp_address, bool sender);
    int accept();

    virtual void close() override;
    virtual void interrupt() override;

    using yarp::os::OutputStream::write;
    virtual void write(const yarp::os::Bytes& b) override;

    using yarp::os::InputStream::read;
    virtual yarp::conf::ssize_t read(const yarp::os::Bytes& b) override;

    // TwoWayStrem implementation
    virtual yarp::os::InputStream& getInputStream() override;
    virtual yarp::os::OutputStream& getOutputStream() override;
    virtual bool isOk() override;

    virtual void reset() override;

    virtual void beginPacket() override;
    virtual void endPacket() override;

    virtual const yarp::os::Contact& getLocalAddress() override;
    virtual const yarp::os::Contact& getRemoteAddress() override;

private:
    enum
    {
        CONNECT = 0,
        ACKNOWLEDGE,
        READ,
        WRITE,
        CLOSE,
        WAKE_UP_MF,
        RESIZE
    };

    // DATA
    bool m_bLinked;

    yarp::os::Contact m_LocalAddress;
    yarp::os::Contact m_RemoteAddress;
    ACE_SOCK_Stream m_SockStream;
    ACE_SOCK_Acceptor m_Acceptor;

    ShmemInputStreamImpl in;
    ShmemOutputStreamImpl out;

    // FUNCTIONS
    int connect(const ACE_INET_Addr& ace_address);
};

#endif // YARP_OS_IMPL_SHMEMHYBRIDSTREAM_H
