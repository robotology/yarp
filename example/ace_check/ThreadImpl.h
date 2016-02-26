/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef _ACECHECK_THREADIMPL_
#define _ACECHECK_THREADIMPL_

#include "Runnable.h"
#include "SemaphoreImpl.h"

#include <ace/Thread.h>



/**
 * An abstraction for a thread of execution.
 */
class ThreadImpl : public Runnable {
public:
    ThreadImpl();
    ThreadImpl(Runnable *target);

    virtual ~ThreadImpl();

    int join(double seconds = -1);
    virtual void run();
    virtual void close();

    // should throw if no success
    virtual bool start();

    bool isClosing();
    
    bool isRunning() {
        return active;
    }

    virtual void beforeStart();
    virtual void afterStart(bool success);

    // call before start
    void setOptions(int stackSize = 0);

    static int getCount();

    // won't be public for long...
    static void changeCount(int delta);

    // get a unique key
    long int getKey();

private:
    int stackSize;
    ACE_hthread_t hid;
    ACE_thread_t id;
    bool active;
    bool closing;
    Runnable *delegate;

    static int threadCount;
    static SemaphoreImpl threadMutex;
};

#endif

