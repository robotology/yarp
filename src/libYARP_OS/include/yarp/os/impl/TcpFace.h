// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_TCPFACE_
#define _YARP2_TCPFACE_

#include  <yarp/os/impl/Face.h>
//#include  <yarp/Semaphore.h>

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
class YARP_OS_impl_API yarp::os::impl::TcpFace : public Face {
public:
    TcpFace() { }

    virtual ~TcpFace();

    virtual bool open(const Address& address);
    virtual void close();
    virtual InputProtocol *read();
    virtual OutputProtocol *write(const Address& address);

    virtual Address getLocalAddress();

    /**
     * This class like all classes except the port objects does
     * not have any notion of running in a multi-threaded environment.
     *
     */

private:
    void closeFace();
    Address address;
    PlatformTcpAcceptor peerAcceptor;
};


#endif
