/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_MACHSEMAPHOREIMPL_H
#define YARP_OS_IMPL_MACHSEMAPHOREIMPL_H

#include <mach/mach_init.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#include <yarp/os/impl/Logger.h>

class YARP_OS_impl_API yarp::os::impl::SemaphoreImpl
{
public:
    SemaphoreImpl(unsigned int initialCount = 1) :
        sema(initialCount)
    {
        int result = semaphore_create(mach_task_self(), &sema, SYNC_POLICY_FIFO, initialCount);
        yAssert(result==KERN_SUCCESS);
    }

    virtual ~SemaphoreImpl()
    {
        semaphore_destroy(mach_task_self(), sema);
    }

    // blocking wait
    void wait()
    {
        semaphore_wait(sema);
    }

    // blocking wait with timeout
    bool waitWithTimeout(double timeout)
    {
        mach_timespec_t	ts = { 0, 0 };
        ts.tv_sec = ts.tv_sec + (int)timeout;
        ts.tv_nsec = (long)((timeout-(int)timeout)*1000000000L+0.5);
        return semaphore_timedwait(sema, ts) == KERN_SUCCESS;
    }

    // polling wait
    bool check()
    {
        mach_timespec_t timeout = { 0, 0 };
        return semaphore_timedwait(sema, timeout) == KERN_SUCCESS;
    }

    // increment
    void post()
    {
        semaphore_signal(sema);
    }

private:
    semaphore_t sema;
};

#endif // YARP_OS_IMPL_MACHSEMAPHOREIMPL_H
