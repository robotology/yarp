/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_RUNNABLE_H
#define YARP_OS_RUNNABLE_H

#include <yarp/conf/system.h>

#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/os/Runnable.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3

#include <yarp/os/api.h>

namespace yarp {
namespace os {

/**
 * A class that can be managed by another thread.
 *
 * @deprecated since YARP 3.3
 */
class YARP_os_DEPRECATED_API Runnable
{
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

} // namespace os
} // namespace yarp

#endif // YARP_NO_DEPRECATED

#endif // YARP_OS_RUNNABLE_H
