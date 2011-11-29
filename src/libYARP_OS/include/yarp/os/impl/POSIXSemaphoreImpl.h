// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick, Miguel Sarabia del Castillo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_POSIXSEMAPHOREIMPL_
#define _YARP2_POSIXSEMAPHOREIMPL_

#include <semaphore.h>

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

