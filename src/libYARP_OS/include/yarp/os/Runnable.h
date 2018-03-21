/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_OS_RUNNABLE_H
#define YARP_OS_OS_RUNNABLE_H

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class Runnable;
    }
}

/**
 * A class that can be managed by another thread.
 */
class YARP_OS_API yarp::os::Runnable {
public:
    /**
     * Destructor
     */
    virtual ~Runnable();

    /**
     * Body to run - could be periodic or continuous
     */
    virtual void run();

    /**
     * User-defined procedure for stopping execution.  There is no
     * general-purpose way to achieve that.
     */
    virtual void close();

    /**
     * Should be called from the creator *before* the thread exists
     * and before a call that requested the thread returns
     */
    virtual void beforeStart();

    /**
     * Should be called from the creator *after* the thread exists
     * and before a call that requested the thread returns
     */
    virtual void afterStart(bool success);

    /**
     * Initialization method. The thread executes this function
     * when it starts and before "run". This is a good place to
     * perform initialization tasks that need to be done by the
     * thread itself (device drivers initialization, memory
     * allocation etc). If the function returns false the thread
     * quits and never calls "run". The return value of threadInit()
     * is notified to the thread class and passed as a parameter
     * to afterStart(). Note that afterStart() is called by the
     * same thread that is executing the start method.
     */
    virtual bool threadInit();

    /**
     * Release method. The thread executes this function once when
     * it exits, after the last "run". This is a good place to release
     * resources that were initialized in threadInit() (release memory,
     * and device driver resources).
     */
    virtual void threadRelease();
};

#endif // YARP_OS_OS_RUNNABLE_H
