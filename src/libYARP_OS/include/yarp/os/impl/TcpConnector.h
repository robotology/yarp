/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_TCPCONNECTOR_H
#define YARP_OS_IMPL_TCPCONNECTOR_H

#if defined(YARP_HAS_ACE)
#  include <ace/config.h>
#  include <ace/SOCK_Connector.h>
#elif defined(__unix__)
#  include <yarp/os/impl/posix/TcpConnector.h>
#else
YARP_COMPILER_ERROR(Cannot implement TcpConnector on this platform)
#endif

namespace yarp {
namespace os {
namespace impl {

#ifdef YARP_HAS_ACE
typedef ACE_SOCK_Connector TcpConnector;
#elif defined(__unix__)
typedef yarp::os::impl::posix::TcpConnector TcpConnector;
#endif

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_TCPCONNECTOR_H
