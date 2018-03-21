/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    explicit ThreadCallbackAdapter(Thread& owner) : owner(owner) {
    }

    virtual ~ThreadCallbackAdapter() {
    }

    virtual void beforeStart() override {
        owner.beforeStart();
    }

    virtual void afterStart(bool success) override {
        owner.afterStart(success);
    }

    virtual void run() override {
        owner.run();
    }

    virtual void close() override {
        if (isRunning()) {
            owner.onStop();
        }
        ThreadImpl::close();
    }

    virtual bool threadInit() override
    {return owner.threadInit();}

    virtual void threadRelease() override
    {owner.threadRelease();}
};


Thread::Thread() :
        implementation(new ThreadCallbackAdapter(*this)),
        stopping(false)
{
    yAssert(implementation!=nullptr);
}


Thread::~Thread() {
    if (implementation) {
        ((ThreadImpl*)implementation)->close();
        delete ((ThreadImpl*)implementation);
        implementation = nullptr;
    }
}


bool Thread::join(double seconds) {
    return ((ThreadImpl*)implementation)->join(seconds) == 0;
}

bool Thread::stop() {
    stopping = true;
    ((ThreadImpl*)implementation)->close();
    return true;
}

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
    YARP_UNUSED(success);
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

long int Thread::getKeyOfCaller() {
    return ThreadImpl::getKeyOfCaller();
}


int Thread::setPriority(int priority, int policy) {
    return ((ThreadImpl*)implementation)->setPriority(priority, policy);
}


int Thread::getPriority() {
    return ((ThreadImpl*)implementation)->getPriority();
}

int Thread::getPolicy() {
    return ((ThreadImpl*)implementation)->getPolicy();
}

void Thread::setDefaultStackSize(int stackSize) {
    ThreadImpl::setDefaultStackSize(stackSize);
}

void Thread::yield() {
    ThreadImpl::yield();
}
