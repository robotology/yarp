/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_TCPCONNECTOR_H
#define YARP_OS_IMPL_TCPCONNECTOR_H

#if defined(YARP_HAS_ACE)
#    include <ace/SOCK_Connector.h>
#    include <ace/config.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#elif defined(__unix__)
#    include <yarp/os/impl/posix/TcpConnector.h>
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
