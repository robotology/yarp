/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_THREADIMPL_H
#define YARP_OS_IMPL_THREADIMPL_H

#include <yarp/os/Semaphore.h>
#include <yarp/os/Runnable.h>

#include <atomic>
#include <thread>

namespace yarp {
namespace os {
namespace impl {

/**
 * An abstraction for a thread of execution.
 */
class YARP_OS_impl_API ThreadImpl : public Runnable
{
public:
    ThreadImpl();
    ThreadImpl(Runnable* target);

    virtual ~ThreadImpl();

    int join(double seconds = -1);
    void run() override;
    void close() override;

    // similar to close, but it does not call join (does not wait for thread termination)
    void askToClose();

    // should throw if no success
    virtual bool start();

    bool isClosing();
    bool isRunning();

    void beforeStart() override;
    void afterStart(bool success) override;

    bool threadInit() override;
    void threadRelease() override;

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

    long tid;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::thread::id) id;

    static void yield();

private:
    int defaultPriority;
    int defaultPolicy;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::thread) thread;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::atomic<bool>) active;
    bool opened;
    bool closing;
    bool needJoin;
    Runnable* delegate;

    yarp::os::Semaphore synchro;

    bool initWasSuccessful;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_THREADIMPL_H
