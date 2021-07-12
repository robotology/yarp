/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Anne van Rossum <anne@almende.com>
 * SPDX-License-Identifier: BSD-3-Clause
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
