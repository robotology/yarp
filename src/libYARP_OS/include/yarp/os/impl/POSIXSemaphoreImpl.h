/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_POSIXSEMAPHOREIMPL_H
#define YARP_OS_IMPL_POSIXSEMAPHOREIMPL_H

#include <semaphore.h>
#include <ctime>

#include <yarp/os/api.h>

class YARP_OS_impl_API yarp::os::impl::SemaphoreImpl
{
public:
    SemaphoreImpl(unsigned int initialCount = 1)
    {
        sem_init(&sema, 0, initialCount);
    }

    virtual ~SemaphoreImpl()
    {
        sem_destroy(&sema);
    }

    // blocking wait
    void wait()
    {
        sem_wait(&sema);
    }

    // blocking wait with timeout
    bool waitWithTimeout(double timeout)
    {
        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1) return false;
        ts.tv_sec = ts.tv_sec + (int)timeout;
        ts.tv_nsec = ts.tv_nsec +
            (long)((timeout-(int)timeout)*1000000000L+0.5);
        if (ts.tv_nsec >= 1000000000L) {
            ts.tv_sec++;
            ts.tv_nsec = ts.tv_nsec - 1000000000L;
        }
        int result = sem_timedwait(&sema, &ts);
        return result == 0;
    }

    // polling wait
    bool check()
    {
        return sem_trywait(&sema)==0;
    }

    // increment
    void post()
    {
        sem_post(&sema);
    }

private:
    sem_t sema;
};

#endif
