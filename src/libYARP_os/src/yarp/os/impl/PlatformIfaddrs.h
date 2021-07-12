/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef YARP_OS_IMPL_PLATFORMIFADDRS_H
#define YARP_OS_IMPL_PLATFORMIFADDRS_H

#include <yarp/conf/system.h>
#if defined(YARP_HAS_IFADDRS_H)
#    include <ifaddrs.h>
#endif

namespace yarp {
namespace os {
namespace impl {

// ACE_OS::getifaddrs, etc are not implemented, anyway ACE implementation
// is different, therefore they are not needed.
#if !defined(YARP_HAS_ACE)
using ::freeifaddrs;
using ::getifaddrs;
#endif

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PLATFORMIFADDRS_H
