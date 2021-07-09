/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PLATFORMNETDB_H
#define YARP_OS_IMPL_PLATFORMNETDB_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
#    include <ace/OS_NS_netdb.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#elif defined(YARP_HAS_NETDB_H)
#    include <netdb.h>
#endif


namespace yarp {
namespace os {
namespace impl {

#ifdef YARP_HAS_ACE
using ACE_OS::gethostbyaddr;
using ACE_OS::gethostbyname;
// ACE_OS::getaddrinfo, etc are not implemented, anyway ACE implementation
// is different, therefore they are not needed.
#else
using ::freeaddrinfo;
using ::gai_strerror;
using ::getaddrinfo;
using ::gethostbyaddr;
using ::gethostbyname;
using ::getnameinfo;
#endif

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_PLATFORMNETDB_H
