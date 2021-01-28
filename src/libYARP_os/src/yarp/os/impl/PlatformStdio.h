/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PLATFORMSTDIO_H
#define YARP_OS_IMPL_PLATFORMSTDIO_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
#    include <ace/OS_NS_stdio.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#else
#    include <stdio.h>
#endif

namespace yarp {
namespace os {
namespace impl {

#ifdef YARP_HAS_ACE
using ACE_OS::fileno;
#else
using ::fileno;
#endif

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_PLATFORMSTDIO_H
