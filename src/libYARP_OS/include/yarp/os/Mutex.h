// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_MUTEX_
#define _YARP2_MUTEX_

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class Mutex;
    }
}


/**
 *
 * Basic wrapper for mutual exclusion.
 *
 */
class YARP_OS_API yarp::os::Mutex {
public:

    /**
     *
     * Constructor.
     *
     */
    Mutex();

    /**
     *
     * Destructor.
     *
     */
    ~Mutex();

    /**
     *
     * Lock the associated resource, waiting if necessary.
     *
     */
    void lock();

    /**
     *
     * Lock the associated resource if possible.  Don't wait.
     *
     * @return true if the associated resource was locked.
     *
     */
    bool tryLock();

    /**
     *
     * Unlock the associated resource.
     *
     */
    void unlock();
private:
    void *implementation;
};


#endif
