// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_SEMAPHORE
#define YARP2_SEMAPHORE

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class Semaphore;
    }
}

/**
 * \ingroup key_class
 *
 * A class for thread synchronization and mutual exclusion.
 * A semaphore has an internal counter.  Multiple threads can
 * safely increment or decrement that counter.  If one thread
 * attempts to decrement the counter below zero, it must wait
 * for another thread to first increment it.  This is a 
 * useful primitive for regulating thread interaction.
 */
class YARP_OS_API yarp::os::Semaphore {
public:

    /**
     * Constructor.  Sets the initial value of the counter.
     * @param initialCount initial value of the counter
     */
    Semaphore(unsigned int initialCount = 1);

    /**
     * Destructor.
     */
    virtual ~Semaphore();

    /**
     * Decrement the counter, even if we must wait to do that.  If the counter
     * would decrement below zero, the calling thread must stop and
     * wait for another thread to call Semaphore::post on this semaphore.
     */
    void wait();


    /**
     * Try to decrement the counter, even if we must wait - but don't wait
     * forever.  This method wiill wait for at most timeoutInSeconds seconds
     * (this can be fractional).  If the counter has not been decremented
     * within that interval, the method will return false.
     *
     * @param timeoutInSeconds the maximum length of time to wait, in seconds (may be fractional).
     * @return true if the counter was decremented, false if a timeout occurred.
     *
     */
    bool waitWithTimeout(double timeoutInSeconds);


    /**
     * Decrement the counter, unless that would require waiting.  If the counter
     * would decrement below zero, this method simply returns without doing
     * anything.
     * @return true if the counter was decremented
     */
    bool check();

    /**
     * Increment the counter.  If another thread is waiting to decrement the
     * counter, it is woken up.
     */
    void post();


private:
    void *implementation;
};

#endif

