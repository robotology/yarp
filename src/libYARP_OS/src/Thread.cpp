// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/Thread.h>
#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class ThreadCallbackAdapter : public ThreadImpl {
private:
    Thread& owner;
public:

    ThreadCallbackAdapter(Thread& owner) : owner(owner) {
    }

    virtual void beforeStart() {
        owner.beforeStart();
    }

    virtual void afterStart(bool success) {
        owner.afterStart(success);
    }

    virtual void run() {
        owner.run();
    }

    virtual void close() {
        owner.onStop();
        ThreadImpl::close();
    }

    virtual bool threadInit() 
    {return owner.threadInit();}

    virtual void threadRelease() 
    {owner.threadRelease();}
};


Thread::Thread() {
    implementation = new ThreadCallbackAdapter(*this);
    YARP_ASSERT(implementation!=NULL);
    stopping = false;
}


Thread::~Thread() {
    if (implementation!=NULL) {
        delete ((ThreadImpl*)implementation);
        implementation = NULL;
    }
}


bool Thread::join(double seconds) {
    bool result = ((ThreadImpl*)implementation)->join(seconds);
    return result;
}

bool Thread::stop() {
    stopping = true;
    ((ThreadImpl*)implementation)->close();
    return true;
}


//void Thread::run() {
//}


void Thread::onStop()
{
    // by default this does nothing
}

bool Thread::start() {
    stopping = false;
    return ((ThreadImpl*)implementation)->start();
}

bool Thread::isStopping() {
    //return ((ThreadImpl*)implementation)->isClosing();
    return stopping;
}

bool Thread::isRunning() {
    return ((ThreadImpl*)implementation)->isRunning();
}

void Thread::beforeStart() {
}

void Thread::afterStart(bool success) {
}


void Thread::setOptions(int stackSize) {
    ((ThreadImpl*)implementation)->setOptions(stackSize);
}

int Thread::getCount() {
    return ThreadImpl::getCount();
}

// get a unique key
long int Thread::getKey() {
    return ((ThreadImpl*)implementation)->getKey();
}
