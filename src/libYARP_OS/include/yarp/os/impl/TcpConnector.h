/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Anne van Rossum <anne@almende.com>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_TCPCONNECTOR_H
#define YARP_OS_IMPL_TCPCONNECTOR_H

#include <yarp/os/Contact.h>
#include <yarp/os/impl/TcpStream.h>


namespace yarp {
    namespace os {
        namespace impl {
            class TcpConnector;
        }
    }
}

/* **************************************************************************************
 * Interface of TcpConnector
 * **************************************************************************************/

class yarp::os::impl::TcpConnector
{
public:
    /**
     * Constructor TcpConnector
     */
    TcpConnector();

    /**
     * Destructor ~TcpConnector
     */
    virtual ~TcpConnector();

    int connect (TcpStream &new_stream,
                 const yarp::os::Contact &remote_address);
protected:

    int open(TcpStream &stream);
private:

};

#endif // YARP_OS_IMPL_TCPCONNECTOR_H
