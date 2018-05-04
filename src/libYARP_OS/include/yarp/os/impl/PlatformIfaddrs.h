/*
 * Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_PLATFORMIFADDRS_H
#define YARP_OS_IMPL_PLATFORMIFADDRS_H

#include <yarp/conf/system.h>
#if defined(YARP_HAS_IFADDRS_H)
# include <ifaddrs.h>
#endif

namespace yarp {
namespace os {
namespace impl {

// ACE_OS::getifaddrs, etc are not implemented, anyway ACE implementation
// is different, therefore they are not needed.
#if !defined(YARP_HAS_ACE)
    using ::getifaddrs;
    using ::freeifaddrs;
#endif

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PLATFORMIFADDRS_H
