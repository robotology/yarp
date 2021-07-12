/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_THREADIMPL_H
#define YARP_OS_IMPL_THREADIMPL_H

#include <yarp/os/Semaphore.h>

#include <atomic>
#include <thread>

namespace yarp {
namespace os {
namespace impl {

/**
 * An abstraction for a thread of execution.
 */
class YARP_os_impl_API ThreadImpl
{
public:
    virtual ~ThreadImpl();

    int join(double seconds = -1);
    virtual void run();
    virtual void close();

    // similar to close, but it does not call join (does not wait for thread termination)
    void askToClose();

    // should throw if no success
    virtual bool start();

    bool isClosing();
    bool isRunning();

    virtual void beforeStart();
    virtual void afterStart(bool success);

    virtual bool threadInit();
    virtual void threadRelease();

    static int getCount();

    // get a unique key
    long int getKey();
    static long int getKeyOfCaller();

    //should become private, when the thread is friend
    void notify(bool s);
    void notifyOpened(bool s)
    {
        opened = s;
    }
    void synchroWait();
    void synchroPost();

    int setPriority(int priority = -1, int policy = -1);
    int getPriority();
    int getPolicy();
    long getTid();

    long tid{-1};
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::thread::id) id;

    static void yield();

private:
    int defaultPriority{-1};
    int defaultPolicy{-1};
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::thread) thread;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::atomic<bool>) active{false};
    bool opened{false};
    bool closing{false};
    bool needJoin{false};

    yarp::os::Semaphore synchro{0};

    bool initWasSuccessful{false};
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_THREADIMPL_H
