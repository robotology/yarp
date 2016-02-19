// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

//added threadInit/threadRelease functions, and synchronization -nat

#ifndef YARP2_THREADIMPL
#define YARP2_THREADIMPL

#include <yarp/os/impl/Runnable.h>
#include <yarp/os/impl/SemaphoreImpl.h>

#include <yarp/os/impl/PlatformThread.h>
#include <yarp/os/impl/String.h>

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
class YARP_OS_impl_API yarp::os::impl::ThreadImpl : public Runnable {
public:

    ThreadImpl();
    ThreadImpl(Runnable *target);

    virtual ~ThreadImpl();

    int join(double seconds = -1);
    virtual void run();
    virtual void close();

    // similar to close, but it does not call join (does not wait for thread termination)
    void askToClose();

    // should throw if no success
    virtual bool start();

    bool isClosing();

    bool isRunning() {
        return active;
    }

    virtual void beforeStart();
    virtual void afterStart(bool success);

    virtual bool threadInit();
    virtual void threadRelease();

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

private:
    int defaultPriority;
    int defaultPolicy;
    int stackSize;
    Platform_hthread_t hid;
    Platform_thread_t  id;
    bool active;
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

#endif
