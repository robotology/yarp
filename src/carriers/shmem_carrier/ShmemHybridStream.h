/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SHMEM_SHMEMHYBRIDSTREAM_H
#define YARP_SHMEM_SHMEMHYBRIDSTREAM_H

#include <yarp/os/InputStream.h>
#include <yarp/os/NetType.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/TwoWayStream.h>

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

    void close() override;
    void interrupt() override;

    using yarp::os::OutputStream::write;
    void write(const yarp::os::Bytes& b) override;

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& b) override;

    // TwoWayStrem implementation
    yarp::os::InputStream& getInputStream() override;
    yarp::os::OutputStream& getOutputStream() override;
    bool isOk() const override;

    void reset() override;

    void beginPacket() override;
    void endPacket() override;

    const yarp::os::Contact& getLocalAddress() const override;
    const yarp::os::Contact& getRemoteAddress() const override;

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

#endif // YARP_SHMEM_SHMEMHYBRIDSTREAM_H
