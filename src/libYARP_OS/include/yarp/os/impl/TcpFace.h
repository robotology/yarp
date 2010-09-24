// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_TCPFACE_
#define _YARP2_TCPFACE_

#include  <yarp/os/impl/Face.h>
//#include  <yarp/Semaphore.h>

#include <ace/config.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>


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
class yarp::os::impl::TcpFace : public Face {
public:
    TcpFace() { }

    virtual ~TcpFace();

    virtual bool open(const Address& address); // throws IOException
    virtual void close(); // throws IOException
    virtual InputProtocol *read(); // throws IOException
    virtual OutputProtocol *write(const Address& address); // throws IOException

    /**
     * This class like all classes except the port objects does
     * not have any notion of running in a multi-threaded environment.
     *
     */

private:
    void closeFace();
    Address address;
    ACE_SOCK_Acceptor peerAcceptor;
};


#endif
