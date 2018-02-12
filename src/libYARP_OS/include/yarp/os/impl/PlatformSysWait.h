/*
 * Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_PLATFORMSYSWAIT_H
#define YARP_OS_IMPL_PLATFORMSYSWAIT_H

#include <yarp/conf/system.h>
#if defined(YARP_HAS_ACE)
# include <ace/OS_NS_sys_wait.h>
#elif defined(YARP_HAS_SYS_WAIT_H)
# include <sys/wait.h>
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
