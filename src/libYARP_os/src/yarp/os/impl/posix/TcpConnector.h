/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Anne van Rossum <anne@almende.com>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_POSIX_TCPCONNECTOR_H
#define YARP_OS_IMPL_POSIX_TCPCONNECTOR_H

#include <yarp/os/Contact.h>
#include <yarp/os/impl/PlatformTime.h>
#include <yarp/os/impl/posix/TcpStream.h>


namespace yarp {
namespace os {
namespace impl {
namespace posix {


/* **************************************************************************************
 * Interface of TcpConnector
 * **************************************************************************************/

class TcpConnector
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
                 const yarp::os::Contact &remote_address,
                 YARP_timeval* timeout = nullptr);
protected:

    int open(TcpStream &stream);
private:

};

} // namespace posix
} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_POSIX_TCPCONNECTOR_H
