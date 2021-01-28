/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Anne van Rossum <anne@almende.com>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_POSIX_TCPACCEPTOR_H
#define YARP_OS_IMPL_POSIX_TCPACCEPTOR_H


#include <yarp/os/TwoWayStream.h>

#include <yarp/os/impl/posix/TcpStream.h>

// General files

namespace yarp {
namespace os {
namespace impl {
namespace posix {


/* **************************************************************************************
 * Interface of TcpAcceptor
 * **************************************************************************************/

class YARP_os_impl_API TcpAcceptor
{
public:
    TcpAcceptor() = default;

    virtual ~TcpAcceptor() = default;

    int open(const yarp::os::Contact& address);

    int connect(const yarp::os::Contact& address);

    int close();

    int accept(TcpStream &new_stream);

    int get_port_number()
    {
        return port_number;
    }

protected:
    int shared_open(const yarp::os::Contact& address);

    int get_handle()
    {
        return ad;
    }

    void set_handle(int h)
    {
        ad = h;
    }

private:
    // acceptor descriptor
    int ad {-1};
    int port_number {-1};
};

} // namespace posix
} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_POSIX_TCPACCEPTOR_H
