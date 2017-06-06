/*
 * Copyright (C) 2007 RobotCub Consortium
 * Author: Alessandro Scalzo <alessandro.scalzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_SHMEMHYBRIDSTREAM_H
#define YARP_OS_IMPL_SHMEMHYBRIDSTREAM_H

#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/NetType.h>

namespace yarp {
    namespace os {
        namespace impl {
            class ShmemHybridStream;
        }
    }
}

#include <yarp/os/impl/ShmemInputStream.h>
#include <yarp/os/impl/ShmemOutputStream.h>


/**
 * A stream abstraction for shared memory communication.
 */
class yarp::os::impl::ShmemHybridStream : public TwoWayStream,
           yarp::os::InputStream,
           yarp::os::OutputStream
{
public:
    ShmemHybridStream() { m_bLinked=false; }
    virtual ~ShmemHybridStream() { close(); }
    int open(const Contact& yarp_address, bool sender);
    int accept();

    virtual void close() override
    {
        m_bLinked=false;
        in.close();
        out.close();
    }

    virtual void interrupt() override
    {
        //printf("INTERRUPT\n");
        //fflush(stdout);
        close();
    }

    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b) override
    {
        if (!out.write(b)) close();
    }

    using yarp::os::InputStream::read;
    virtual YARP_SSIZE_T read(const Bytes& b) override
    {
        YARP_SSIZE_T ret=in.read(b);
        if (ret==-1) close();
        return ret;
    }

    // TwoWayStrem implementation
    virtual yarp::os::InputStream& getInputStream() override { return *this; }
    virtual yarp::os::OutputStream& getOutputStream() override { return *this; }
    virtual bool isOk() override { return m_bLinked && in.isOk() && out.isOk(); }

    virtual void reset() override
    {
        //printf("RECEIVED RESET COMMAND\n");
        //fflush(stdout);
        close();
    }

    virtual void beginPacket() override {}
    virtual void endPacket() override {}

    virtual const Contact& getLocalAddress() override { return m_LocalAddress; }
    virtual const Contact& getRemoteAddress() override { return m_RemoteAddress; }

protected:
    enum {CONNECT=0, ACKNOWLEDGE, READ, WRITE, CLOSE, WAKE_UP_MF, RESIZE};

    // DATA

    bool m_bLinked;

    Contact m_LocalAddress, m_RemoteAddress;
    ACE_SOCK_Stream m_SockStream;
    ACE_SOCK_Acceptor m_Acceptor;

    ShmemInputStreamImpl in;
    ShmemOutputStreamImpl out;

    // FUNCTIONS
    int connect(const ACE_INET_Addr &ace_address);
};

#endif // YARP_OS_IMPL_SHMEMHYBRIDSTREAM_H
