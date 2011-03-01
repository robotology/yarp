// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SEMAPHOREIMPL_
#define _YARP2_SEMAPHOREIMPL_

// There is a problem with YARP+ACE semaphores on some Linux distributions,
// where semaphores fail to work correctly.
// A hack to fix this problem is to uncomment the three lines below --paulfitz
#ifdef __linux__
#define YARP_USE_NATIVE_POSIX_SEMA
#endif

#ifdef YARP_USE_NATIVE_POSIX_SEMA
#include <semaphore.h>
#endif

#include <ace/Synch.h>

#include <yarp/os/impl/String.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NetType.h>

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
#ifndef YARP_USE_NATIVE_POSIX_SEMA
        : sema(initialCount) 
#endif
    {
#ifdef YARP_USE_NATIVE_POSIX_SEMA
        sem_init(&sema,0,initialCount);
#endif
    }

    virtual ~SemaphoreImpl() {
#ifdef YARP_USE_NATIVE_POSIX_SEMA
        sem_destroy(&sema);
#endif
    }

    // blocking wait
    void wait() {
#ifdef YARP_USE_NATIVE_POSIX_SEMA
        sem_wait(&sema);
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
#ifdef YARP_USE_NATIVE_POSIX_SEMA
        return sem_trywait(&sema)==0;
#else
        return (sema.tryacquire()<0)?0:1;
#endif
    }

    // increment
    void post() {
#ifdef YARP_USE_NATIVE_POSIX_SEMA
        sem_post(&sema);
#else
        sema.release();
#endif
    }

private:
#ifdef YARP_USE_NATIVE_POSIX_SEMA
    sem_t sema;
#else
    ACE_Semaphore sema;
#endif
};

#endif

