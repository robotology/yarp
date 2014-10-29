// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick, Miguel Sarabia del Castillo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_MACHSEMAPHOREIMPL_
#define _YARP2_MACHSEMAPHOREIMPL_

#include <mach/mach_init.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#include <yarp/os/impl/Logger.h>

class YARP_OS_impl_API yarp::os::impl::SemaphoreImpl {
public:
    SemaphoreImpl(unsigned int initialCount = 1)
        :sema(initialCount)
    {
        int result = semaphore_create(mach_task_self(), &sema, SYNC_POLICY_FIFO, initialCount);
	    yAssert(result==KERN_SUCCESS);
    }

    virtual ~SemaphoreImpl() {
        semaphore_destroy(mach_task_self(),sema);
    }

    // blocking wait
    void wait() {
        semaphore_wait(sema);
    }

    // blocking wait with timeout
    bool waitWithTimeout(double timeout) {
        mach_timespec_t	ts = { 0, 0 };
        ts.tv_sec = ts.tv_sec + (int)timeout;
        ts.tv_nsec = (long)((timeout-(int)timeout)*1000000000L+0.5);
        return semaphore_timedwait(sema,ts) == KERN_SUCCESS;
    }

    // polling wait
    bool check() {
        mach_timespec_t timeout = { 0, 0 };
        return semaphore_timedwait(sema,timeout) == KERN_SUCCESS;
    }

    // increment
    void post() {
        semaphore_signal(sema);
    }

private:
    semaphore_t sema;
};

#endif

