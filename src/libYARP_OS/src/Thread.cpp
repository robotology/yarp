// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/Thread.h>
#include <yarp/ThreadImpl.h>
#include <yarp/Logger.h>

using namespace yarp;
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
};


Thread::Thread() {
    implementation = new ThreadCallbackAdapter(*this);
    YARP_ASSERT(implementation!=NULL);
}


Thread::~Thread() {
    if (implementation!=NULL) {
        delete ((ThreadImpl*)implementation);
        implementation = NULL;
    }
}


bool Thread::join(double seconds) {
    return ((ThreadImpl*)implementation)->join(seconds);
}

bool Thread::stop() {
    return ((ThreadImpl*)implementation)->join(-1);
}


void Thread::run() {
}


void Thread::onStop() {
    ((ThreadImpl*)implementation)->close();
}

bool Thread::start() {
    return ((ThreadImpl*)implementation)->start();
}

bool Thread::isStopping() {
    return ((ThreadImpl*)implementation)->isClosing();
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
