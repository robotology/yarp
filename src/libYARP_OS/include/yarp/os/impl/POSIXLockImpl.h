// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 Robotics and Cognitive Sciences Department. IIT
 * Authors: Francesco Romano
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_POSIXLOCKIMPL_H
#define YARP_POSIXLOCKIMPL_H

#include <pthread.h>
#include <yarp/os/impl/Logger.h>

class YARP_OS_impl_API yarp::os::impl::RecursiveLockImpl {
public:
    RecursiveLockImpl()
    {
        pthread_mutexattr_t attributes;
        yAssert(pthread_mutexattr_init(&attributes) == 0);
        yAssert(pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE) == 0);
        yAssert(pthread_mutex_init(&mutex, &attributes) == 0);
        yAssert(pthread_mutexattr_destroy(&attributes) == 0);
    }

    virtual ~RecursiveLockImpl() {
        pthread_mutex_destroy(&mutex);
    }

    // blocking wait
    void lock() {
        pthread_mutex_lock(&mutex);
    }

    bool tryLock() {
        return pthread_mutex_trylock(&mutex) == 0;
    }

    // increment
    void unlock() {
        pthread_mutex_unlock(&mutex);
    }

private:
    pthread_mutex_t mutex;
};


#endif /* end of include guard: YARP_POSIXLOCKIMPL_H */
