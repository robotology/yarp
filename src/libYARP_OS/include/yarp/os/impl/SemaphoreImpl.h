/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_SEMAPHOREIMPL_H
#define YARP_OS_IMPL_SEMAPHOREIMPL_H


// Pre-declare semaphore
namespace yarp {
    namespace os {
        namespace impl {
            class SemaphoreImpl;
        }
    }
}

//Decide which implementation to use
#include <yarp/conf/system.h>

#if defined(YARP_HAS_CXX11)
#  include <yarp/os/impl/CXX11SemaphoreImpl.h>
#elif defined(__unix__)
// There is a problem with YARP+ACE semaphores on some Linux distributions,
// where semaphores fail to work correctly.  Workaround.
#  include <yarp/os/impl/POSIXSemaphoreImpl.h>
# elif defined(__APPLE__)
// On Mac, POSIX semaphores are just too burdensome
#  include <yarp/os/impl/MachSemaphoreImpl.h>
# elif defined(YARP_HAS_ACE)
// For everything else, there's ACE
# include <yarp/os/impl/ACESemaphoreImpl.h>
# else
YARP_COMPILER_ERROR(Cannot implement semaphores on this platform)
#endif

#endif // YARP_OS_IMPL_SEMAPHOREIMPL_H
