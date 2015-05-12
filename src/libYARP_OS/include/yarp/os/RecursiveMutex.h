// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 Robotics and Cognitive Sciences Department. IIT
 * Authors: Francesco Romano
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_RECURSIVELOCK_H
#define YARP_RECURSIVELOCK_H

#include <yarp/os/api.h>
    
namespace yarp {
    namespace os {
        class RecursiveMutex;
    }
}

/**
 * RecursiveMutex offers exclusive, recursive ownership semantics:
 *   - A calling thread owns a RecursiveMutex for a period of time that starts when it successfully calls either lock or tryLock. During this period, the thread may make additional calls to lock or tryLock. The period of ownership ends when the thread makes a matching number of calls to unlock.
 *   - When a thread owns a RecursiveMutex, all other threads will block (for calls to lock) or receive a false return value (for tryLock) if they attempt to claim ownership of the RecursiveLock.
 * The behavior of a program is undefined if a RecursiveMutex is destroyed while still owned by some thread.
 * The behavior of a program is undefined if a RecursiveMutex is unlocked by a thread which is not
 * currently owning the RecursiveMutex
 *
 */
class YARP_OS_API yarp::os::RecursiveMutex {
public:

    /**
     *
     * Constructor.
     *
     */
    RecursiveMutex();

    /**
     *
     * Destructor.
     *
     */
    ~RecursiveMutex();

    /**
     * @brief Lock the associated resource, waiting if the resource is busy.
     *
     * If the thread which is currently owning the resource calls 
     * this function, it will not block, and a reference count will be increased
     * Thu number of calls to lock() must be balanced by the same number of 
     * calls to unlock()
     *
     */
    void lock();

    /**
     * @brief Lock the associated resource if it is free.
     *
     * @see RecursiveLock#lock() for more detailed description
     * @return true if the associated resource was successfully locked. False otherwise
     *
     */
    bool tryLock();

    /**
     * @brief Unlock the associated resource thus freeing waiting threads.
     *
     * If the resource is not currently locked by the calling thread,
     * the behavior is undefined.
     *
     */
    void unlock();
    
private:
    void *implementation;
};


#endif /* end of include guard: YARP_RECURSIVELOCK_H */

