// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick, Miguel Sarabia del Castillo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_POSIXSEMAPHOREIMPL
#define YARP2_POSIXSEMAPHOREIMPL

#include <semaphore.h>
#include <time.h>

class YARP_OS_impl_API yarp::os::impl::SemaphoreImpl {
public:
    SemaphoreImpl(unsigned int initialCount = 1) {
        sem_init(&sema,0,initialCount);
    }

    virtual ~SemaphoreImpl() {
        sem_destroy(&sema);
    }

    // blocking wait
    void wait() {
        sem_wait(&sema);
    }

    // blocking wait with timeout
    bool waitWithTimeout(double timeout) {
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
    bool check() {
        return sem_trywait(&sema)==0;
    }

    // increment
    void post() {
        sem_post(&sema);
    }

private:
    sem_t sema;
};

#endif

