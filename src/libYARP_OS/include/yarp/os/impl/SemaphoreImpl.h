// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_SEMAPHOREIMPL_
#define _YARP2_SEMAPHOREIMPL_

#include <ace/Synch.h>

#include <yarp/os/impl/Logger.h>

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
class yarp::os::impl::SemaphoreImpl {
public:
    SemaphoreImpl(int initialCount = 1) : sema(initialCount) {
    }

    virtual ~SemaphoreImpl() {}

    // blocking wait
    void wait() {
        int result = sema.acquire();
        while (result == -1) {
            YARP_ERROR(Logger::get(), "semaphore wait failed - gdb problem, or bad YARP+ACE flags");
            result = sema.acquire();
        }
    }

    // polling wait
    bool check() {
        return (sema.tryacquire()<0)?0:1;
    }

    // increment
    void post() {
        sema.release();
    }

private:
    ACE_Semaphore sema;
};

#endif

