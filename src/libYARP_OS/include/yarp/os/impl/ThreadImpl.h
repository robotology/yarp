/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_THREADIMPL_H
#define YARP_OS_IMPL_THREADIMPL_H

#include <yarp/os/impl/Runnable.h>
#include <yarp/os/impl/SemaphoreImpl.h>

#include <yarp/os/impl/PlatformThread.h>

namespace yarp {
    namespace os {
        namespace impl {
            class ThreadImpl;
        }
    }
}

/**
 * An abstraction for a thread of execution.
 */
class YARP_OS_impl_API yarp::os::impl::ThreadImpl : public Runnable
{
public:

    ThreadImpl();
    ThreadImpl(Runnable *target);

    virtual ~ThreadImpl();

    int join(double seconds = -1);
    virtual void run() override;
    virtual void close() override;

    // similar to close, but it does not call join (does not wait for thread termination)
    void askToClose();

    // should throw if no success
    virtual bool start();

    bool isClosing();
    bool isRunning();

    virtual void beforeStart() override;
    virtual void afterStart(bool success) override;

    virtual bool threadInit() override;
    virtual void threadRelease() override;

    // call before start
    void setOptions(int stackSize = 0);

    static int getCount();

    // won't be public for long...
    static void changeCount(int delta);

    // get a unique key
    long int getKey();
    static long int getKeyOfCaller();

    //should become private, when the thread is friend
    void notify(bool s);
    void notifyOpened(bool s) { opened = s; }
    void synchroWait();
    void synchroPost();

    int setPriority(int priority = -1, int policy = -1);
    int getPriority();
    int getPolicy();
    long getTid();

    static void setDefaultStackSize(int stackSize);

    static SemaphoreImpl *threadMutex;
    static SemaphoreImpl *timeMutex; // Used by yarp::os::Time

    static void init();
    static void fini();

    long tid;
    Platform_thread_t  id;

    static void yield();

private:
    int defaultPriority;
    int defaultPolicy;
    int stackSize;
    Platform_hthread_t hid;
    bool active; // FIXME should be atomic
    bool opened;
    bool closing;
    bool needJoin;
    Runnable *delegate;

    SemaphoreImpl synchro;
    //ACE_Auto_Event synchro;   // event for init synchro

    static int threadCount;
    static int defaultStackSize;
    bool initWasSuccessful;

};

#endif // YARP_OS_IMPL_THREADIMPL_H
