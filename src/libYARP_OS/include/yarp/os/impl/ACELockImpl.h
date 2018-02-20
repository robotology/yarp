/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_ACELOCKIMPL_H
#define YARP_OS_IMPL_ACELOCKIMPL_H

#include <ace/Recursive_Thread_Mutex.h>
#include <yarp/os/LogStream.h>

class YARP_OS_impl_API yarp::os::impl::RecursiveLockImpl
{
public:
    void lock()
    {
        int result = mutex.acquire();
        if (result != -1) {
            return;
        }
        yError("Mutex lock failed (errno %d)", ACE_OS::last_error());
    }

    bool tryLock()
    {
        int result = mutex.tryacquire();
        if (result != -1) {
            return true;
        }
        //if errno is EBUSY this is a "clean" failure: lock is busy
        if (ACE_OS::last_error() == EBUSY) {
            return false;
        }
        //different error: return false and print error
        yError("Mutex tryLock failed (errno %d)", ACE_OS::last_error());
        return false;
    }

    // unlock
    void unlock()
    {
        mutex.release();
    }

private:
    ACE_Recursive_Thread_Mutex mutex;
};

#endif // YARP_OS_IMPL_ACELOCKIMPL_H
