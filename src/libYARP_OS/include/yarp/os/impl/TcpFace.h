/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_TCPFACE_H
#define YARP_OS_IMPL_TCPFACE_H

#include <yarp/conf/system.h>
#include <yarp/os/Face.h>
#include <yarp/os/impl/AuthHMAC.h>

#ifdef YARP_HAS_ACE
#  include <ace/config.h>
#  include <ace/SOCK_Acceptor.h>
#  include <ace/SOCK_Connector.h>
#  include <ace/SOCK_Stream.h>
#  include <ace/Log_Msg.h>
#  define PlatformTcpAcceptor ACE_SOCK_Acceptor
#else
#  include <yarp/os/impl/TcpAcceptor.h>
#  define PlatformTcpAcceptor TcpAcceptor
#endif


namespace yarp {
    namespace os {
        namespace impl {
            class TcpFace;
        }
    }
}

/**
 * Communicating with a port via TCP.
 */
class YARP_OS_impl_API yarp::os::impl::TcpFace : public yarp::os::Face
{
public:
    TcpFace() { }

    virtual ~TcpFace();

    virtual bool open(const Contact& address) override;
    virtual void close() override;
    virtual InputProtocol *read() override;
    virtual OutputProtocol *write(const Contact& address) override;

    virtual Contact getLocalAddress() override;

    /**
     * This class like all classes except the port objects does
     * not have any notion of running in a multi-threaded environment.
     *
     */

protected:

    yarp::os::impl::AuthHMAC auth;

private:
    void closeFace();
    Contact address;
    PlatformTcpAcceptor peerAcceptor;
};


#endif // YARP_OS_IMPL_TCPFACE_H
