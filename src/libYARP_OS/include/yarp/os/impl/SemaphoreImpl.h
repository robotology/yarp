// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SEMAPHOREIMPL_
#define _YARP2_SEMAPHOREIMPL_

// There is a problem with YARP+ACE semaphores on some Linux distributions,
// where semaphores fail to work correctly.  Workaround.
#ifdef __linux__
#define YARP_USE_NATIVE_POSIX_SEMA 1
#else
#define YARP_USE_NATIVE_POSIX_SEMA 0
#endif

// On Mac, POSIX semaphores are just too burdensome
#ifdef __APPLE__
#  define YARP_USE_MACH_SEMA 1
#endif

#include <yarp/conf/system.h>
#ifndef YARP_HAS_ACE
#  ifdef __APPLE__
#    ifndef YARP_USE_MACH_SEMA
#      define YARP_USE_MACH_SEMA 1
#    endif
#  endif
#endif

#if YARP_USE_NATIVE_POSIX_SEMA
#  include <semaphore.h>
#endif

#ifdef YARP_USE_MACH_SEMA
#  include <mach/mach_init.h>
#  include <mach/semaphore.h>
#  include <mach/task.h>
#else
#  define YARP_USE_MACH_SEMA 0
#endif

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
#  include <ace/Synch.h>
#endif

#include <yarp/os/impl/String.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/PlatformStdlib.h>

namespace yarp {
    namespace os {
        namespace impl {
            class SemaphoreImpl;
        }
    }
}

/**
 * A semaphore abstraction for mutual exclusion and resource management.
 */
class YARP_OS_impl_API yarp::os::impl::SemaphoreImpl {
public:
    SemaphoreImpl(int initialCount = 1) 
#if !(YARP_USE_NATIVE_POSIX_SEMA)
        : sema(initialCount) 
#endif
    {
#if YARP_USE_NATIVE_POSIX_SEMA
        sem_init(&sema,0,initialCount);
#elif YARP_USE_MACH_SEMA
        int result = semaphore_create(mach_task_self(), &sema, SYNC_POLICY_FIFO, initialCount);
	YARP_ASSERT(result==KERN_SUCCESS);
#endif
    }

    virtual ~SemaphoreImpl() {
#if YARP_USE_NATIVE_POSIX_SEMA
        sem_destroy(&sema);
#elif YARP_USE_MACH_SEMA
        semaphore_destroy(mach_task_self(),sema);
#endif
    }

    // blocking wait
    void wait() {
#if YARP_USE_NATIVE_POSIX_SEMA
        sem_wait(&sema);
#elif YARP_USE_MACH_SEMA
        semaphore_wait(sema);
#else
        int result = sema.acquire();
        if (result!=-1) return;
        int ct = 100;
        while (result == -1 && ct>=0) {
            YARP_ERROR(Logger::get(), yarp::os::impl::String("semaphore wait failed (errno ") + (yarp::os::impl::NetType::toString(ACE_OS::last_error())) + yarp::os::impl::String("); gdb problem, or bad YARP+ACE flags"));
            result = sema.acquire();
            ct--;
        }
        if (result==-1) {
            YARP_ERROR(Logger::get(), "semaphore wait failed");
        } else {
            YARP_ERROR(Logger::get(), "semaphore wait eventually succeeded");
        }
#endif
    }

    // polling wait
    bool check() {
#if YARP_USE_NATIVE_POSIX_SEMA
        return sem_trywait(&sema)==0;
#elif YARP_USE_MACH_SEMA
        mach_timespec_t timeout = { 0, 0 };
        return semaphore_timedwait(sema,timeout) == KERN_SUCCESS;
#else
        return (sema.tryacquire()<0)?0:1;
#endif
    }

    // increment
    void post() {
#if YARP_USE_NATIVE_POSIX_SEMA
        sem_post(&sema);
#elif YARP_USE_MACH_SEMA
        semaphore_signal(sema);
#else
        sema.release();
#endif
    }

private:
#if YARP_USE_NATIVE_POSIX_SEMA
    sem_t sema;
#elif YARP_USE_MACH_SEMA
    semaphore_t sema;
#else
    ACE_Semaphore sema;
#endif
};

#endif

