/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PLATFORMSYSWAIT_H
#define YARP_OS_IMPL_PLATFORMSYSWAIT_H

#include <yarp/conf/system.h>
#if defined(YARP_HAS_ACE)
#    include <ace/OS_NS_sys_wait.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#elif defined(YARP_HAS_SYS_WAIT_H)
#    include <sys/wait.h>
#endif

namespace yarp {
namespace os {
namespace impl {

#if defined(YARP_HAS_ACE)
using ACE_OS::wait;
using ACE_OS::waitpid;
#else
using ::wait;
using ::waitpid;
#endif

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PLATFORMSYSWAIT_H
