// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Francesco Romano
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_LOCKGUARD_
#define _YARP2_LOCKGUARD_

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        template <typename Lockable>
        class AbstractLockGuard;

        class Mutex;
        class RecursiveMutex;

        typedef AbstractLockGuard<Mutex> LockGuard;
        typedef AbstractLockGuard<RecursiveMutex> RecursiveLockGuard;
    }
}

/**
 * This class is a mutex wrapper that provides a convenient RAII-style mechanism for owning
 * a mutex for the duration of a scoped block. When a LockGuard object is created, 
 * it attempts to take ownership of the mutex it is given. 
 * When control leaves the scope in which the LockGuard object was created, 
 * the LockGuard is destructed and the mutex is released.
 * The lock_guard class is non-copyable. 
 */
template <typename Lockable>
class yarp::os::AbstractLockGuard {
public:
    /**
     * Acquires ownership of the given mutex _mutex.
     * The behavior is undefined if _mutex is destroyed before the LockGuard object is.
     * @param _mutex the mutex which will be locked
     */
    explicit AbstractLockGuard(Lockable& _lock);
    
    /**
     * destructs the LockGuard object, unlocks the underlying mutex
     */
    ~AbstractLockGuard();
    
private:
    
    /** Copy constructor is disabled */
    AbstractLockGuard(const AbstractLockGuard&);
    
    /** Assignment operator is disabled */
    AbstractLockGuard& operator=(const AbstractLockGuard&);
    
    Lockable& lock; /*!< underlining mutex */
};


//Implementation
template <typename Lockable>
yarp::os::AbstractLockGuard<Lockable>::AbstractLockGuard(Lockable& _lock)
    : lock(_lock)
{
    lock.lock();
}

template <typename Lockable>
yarp::os::AbstractLockGuard<Lockable>::~AbstractLockGuard()
{
    lock.unlock();
}

template <typename Lockable>
yarp::os::AbstractLockGuard<Lockable>::AbstractLockGuard(const AbstractLockGuard& lg)
: lock(lg.lock) { }

template <typename Lockable>
yarp::os::AbstractLockGuard<Lockable>& yarp::os::AbstractLockGuard<Lockable>::operator=(const AbstractLockGuard&) { return *this; }

#endif
