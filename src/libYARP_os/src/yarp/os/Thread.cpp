/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Thread.h>

#include <yarp/os/impl/ThreadImpl.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class yarp::os::Thread::Private : public ThreadImpl
{
private:
    Thread& owner;

public:
    explicit Private(Thread& owner) :
            owner(owner),
            stopping(false)
    {
    }

    ~Private() override = default;

    void beforeStart() override
    {
        owner.beforeStart();
    }

    void afterStart(bool success) override
    {
        owner.afterStart(success);
    }

    void run() override
    {
        owner.run();
    }

    void close() override
    {
        if (isRunning()) {
            owner.onStop();
        }
        ThreadImpl::close();
    }

    bool threadInit() override
    {
        return owner.threadInit();
    }

    void threadRelease() override
    {
        owner.threadRelease();
    }

    bool stopping;
};


Thread::Thread() :
        mPriv(new Private(*this))
{
}

Thread::~Thread()
{
    mPriv->close();
    delete mPriv;
}

bool Thread::join(double seconds)
{
    return mPriv->join(seconds) == 0;
}

bool Thread::stop()
{
    mPriv->stopping = true;
    mPriv->close();
    return true;
}

void Thread::onStop()
{
    // by default this does nothing
}

bool Thread::start()
{
    mPriv->stopping = false;
    return mPriv->start();
}

bool Thread::isStopping()
{
    //return mPriv->isClosing();
    return mPriv->stopping;
}

bool Thread::isRunning()
{
    return mPriv->isRunning();
}

void Thread::beforeStart()
{
}

void Thread::afterStart(bool success)
{
    YARP_UNUSED(success);
}

int Thread::getCount()
{
    return ThreadImpl::getCount();
}

// get a unique key
long int Thread::getKey()
{
    return mPriv->getKey();
}

long int Thread::getKeyOfCaller()
{
    return ThreadImpl::getKeyOfCaller();
}

int Thread::setPriority(int priority, int policy)
{
    return mPriv->setPriority(priority, policy);
}

int Thread::getPriority()
{
    return mPriv->getPriority();
}

int Thread::getPolicy()
{
    return mPriv->getPolicy();
}

void Thread::yield()
{
    ThreadImpl::yield();
}
