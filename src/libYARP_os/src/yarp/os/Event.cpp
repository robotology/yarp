/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Event.h>
#include <yarp/os/Semaphore.h>

#include <mutex>

class yarp::os::Event::Private
{
public:
    Private(bool autoReset) :
            autoReset(autoReset),
            action(0)
    {
        signalled = false;
        waiters = 0;
    }

    void wait()
    {
        stateMutex.lock();
        if (signalled) {
            stateMutex.unlock();
            return;
        }
        waiters++;
        stateMutex.unlock();
        action.wait();
        if (autoReset) {
            reset();
        }
    }

    void signal(bool after = true)
    {
        stateMutex.lock();
        int w = waiters;
        if (w > 0) {
            if (autoReset) {
                w = 1;
            }
            for (int i = 0; i < w; i++) {
                action.post();
                waiters--;
            }
        }
        signalled = after;
        stateMutex.unlock();
    }

    void reset()
    {
        stateMutex.lock();
        signalled = false;
        stateMutex.unlock();
    }

private:
    bool autoReset;
    bool signalled;
    int waiters;
    std::mutex stateMutex;
    Semaphore action;
};


yarp::os::Event::Event(bool autoResetAfterWait) :
        mPriv(new Private(autoResetAfterWait))
{
}

yarp::os::Event::~Event()
{
    delete mPriv;
}

void yarp::os::Event::wait()
{
    mPriv->wait();
}

void yarp::os::Event::signal()
{
    mPriv->signal();
}

void yarp::os::Event::reset()
{
    mPriv->reset();
}
