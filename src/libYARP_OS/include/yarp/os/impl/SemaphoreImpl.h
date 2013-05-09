// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2009 RobotCub Consortium
 * Authors: Miguel Sarabia del Castillo, Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SEMAPHOREIMPL_
#define _YARP2_SEMAPHOREIMPL_

#include <yarp/os/api.h>
//Decide which implementation to use
#include <yarp/conf/system.h>

#if defined(__linux__)
// There is a problem with YARP+ACE semaphores on some Linux distributions,
// where semaphores fail to work correctly.  Workaround.
#  include <semaphore.h>
#elif defined(__APPLE__)
// On Mac, POSIX semaphores are just too burdensome
#  include <mach/semaphore.h>
#elif defined(YARP_HAS_ACE)
// For everything else, there's ACE
#  include <ace/Synch.h>
#else
#  error Cannot implement semaphores
#endif

// Pre-declare semaphore
namespace yarp {
    namespace os {
        namespace impl {
            class SemaphoreImpl;
        }
    }
}



class YARP_OS_impl_API yarp::os::impl::SemaphoreImpl {
public:
    SemaphoreImpl(unsigned int initialCount = 1);
    virtual ~SemaphoreImpl();

    // blocking wait
    void wait();

    // blocking wait with timeout
    bool waitWithTimeout(double timeout);

    // polling wait
    bool check();

    // increment
    void post();

private:

#if defined(__linux__)
    sem_t sema;
#elif defined(__APPLE__)
    semaphore_t sema;
#elif defined(YARP_HAS_ACE)
    ACE_Semaphore sema;
#endif

};

#endif

