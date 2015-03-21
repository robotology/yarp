// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 Robotics and Cognitive Sciences Department. IIT
 * Authors: Francesco Romano
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_ACELOCKIMPL_H
#define YARP_ACELOCKIMPL_H

#include <ace/Recursive_Thread_Mutex.h>
#include <yarp/os/LogStream.h>

class YARP_OS_impl_API yarp::os::impl::RecursiveLockImpl {
public:
    void lock() {
        int result = mutex.acquire();
        if (result != -1) return;
        yError("Mutex lock failed (errno %d)", ACE_OS::last_error());
    }

    bool tryLock() {
        int result = mutex.tryacquire();
        if (result != -1) return true;
        //if errno is EBUSY this is a "clean" failure: lock is busy
        if (ACE_OS::last_error() == EBUSY) return false;
        //different error: return false and print error
        yError("Mutex tryLock failed (errno %d)", ACE_OS::last_error());
        return false;

    }

    // unlock
    void unlock() {
        mutex.release();
    }

private:
    ACE_Recursive_Thread_Mutex mutex;
};

#endif /* end of include guard: YARP_ACELOCKIMPL_H */
