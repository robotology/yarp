/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_TCPSTREAM_H
#define YARP_OS_IMPL_TCPSTREAM_H

#if defined(YARP_HAS_ACE)
#    include <ace/SOCK_Stream.h>
#    include <ace/config.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#elif defined(__unix__)
#    include <yarp/os/impl/posix/TcpStream.h>
#else
YARP_COMPILER_ERROR(Cannot implement TcpStream on this platform)
#endif

namespace yarp {
namespace os {
namespace impl {

#ifdef YARP_HAS_ACE
typedef ACE_SOCK_Stream TcpStream;
#elif defined(__unix__)
typedef yarp::os::impl::posix::TcpStream TcpStream;
#endif

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_TCPSTREAM_H
