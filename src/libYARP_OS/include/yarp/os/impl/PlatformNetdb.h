/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PLATFORMNETDB_H
#define YARP_OS_IMPL_PLATFORMNETDB_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
# include <ace/OS_NS_netdb.h>
#elif defined(YARP_HAS_NETDB_H)
# include <netdb.h>
#endif


namespace yarp {
namespace os {
namespace impl {

#ifdef YARP_HAS_ACE
    using ACE_OS::gethostbyaddr;
    using ACE_OS::gethostbyname;
    // ACE_OS::getaddrinfo, etc are not implemented, anyway ACE implementation
    // is different, therefore they are not neeeded.
#else
    using ::gethostbyaddr;
    using ::gethostbyname;
    using ::getaddrinfo;
    using ::freeaddrinfo;
    using ::gai_strerror;
    using ::getnameinfo;
#endif

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_PLATFORMNETDB_H
