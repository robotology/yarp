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
        class Mutex;
        class LockGuard;
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
class YARP_OS_API yarp::os::LockGuard {
public:
    /**
     * Acquires ownership of the given mutex _mutex.
     * The behavior is undefined if _mutex is destroyed before the LockGuard object is.
     * @param _mutex the mutex which will be locked
     */
    explicit LockGuard(yarp::os::Mutex& _mutex);
    
    /**
     * destructs the LockGuard object, unlocks the underlying mutex
     */
    ~LockGuard();
    
private:
    
    /** Copy constructor is disabled */
    LockGuard(const LockGuard&);
    
    /** Assignment operator is disabled */
    LockGuard& operator=(const LockGuard&);
    
    yarp::os::Mutex& mutex; /*!< underlining mutex */
};

#endif
