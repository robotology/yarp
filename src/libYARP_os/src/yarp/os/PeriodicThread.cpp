/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/PeriodicThread.h>

#include <yarp/os/SystemClock.h>
#include <yarp/os/impl/PlatformTime.h>
#include <yarp/os/impl/ThreadImpl.h>

#include <cmath>
#include <mutex>

using namespace yarp::os::impl;
using namespace yarp::os;


class yarp::os::PeriodicThread::Private : public ThreadImpl
{
private:
    double adaptedPeriod;
    PeriodicThread* owner;
    mutable std::mutex mutex;

    double elapsed;
    double sleepPeriod;

    bool suspended;
    double totalUsed;    //total time taken iterations
    unsigned int count;  //number of iterations from last reset
    unsigned int estPIt; //number of useful iterations for period estimation
    double totalT;       //time bw run, accumulated
    double sumTSq;       //cumulative sum sq of estimated period dT
    double sumUsedSq;    //cumulative sum sq of estimated thread tun
    double previousRun;  //time when last iteration started
    double currentRun;   //time when this iteration started
    bool scheduleReset;

    using NowFuncPtr = double (*)();
    using DelayFuncPtr = void (*)(double);
    const NowFuncPtr nowFunc;
    const DelayFuncPtr delayFunc;

    void _resetStat()
    {
        totalUsed = 0;
        count = 0;
        estPIt = 0;
        totalT = 0;
        sumUsedSq = 0;
        sumTSq = 0;
        elapsed = 0;
        scheduleReset = false;
    }

public:
    Private(PeriodicThread* owner, double p, ShouldUseSystemClock useSystemClock) :
            adaptedPeriod(p),
            owner(owner),
            elapsed(0),
            sleepPeriod(adaptedPeriod),
            suspended(false),
            totalUsed(0),
            count(0),
            estPIt(0),
            totalT(0),
            sumTSq(0),
            sumUsedSq(0),
            previousRun(0),
            currentRun(0),
            scheduleReset(false),
            nowFunc(useSystemClock == ShouldUseSystemClock::Yes ? SystemClock::nowSystem : yarp::os::Time::now),
            delayFunc(useSystemClock == ShouldUseSystemClock::Yes ? SystemClock::delaySystem : yarp::os::Time::delay)
    {
    }

    void resetStat()
    {
        scheduleReset = true;
    }

    double getEstimatedPeriod() const
    {
        double ret;
        lock();
        if (estPIt == 0) {
            ret = 0;
        } else {
            ret = (totalT / estPIt);
        }
        unlock();
        return ret;
    }

    void getEstimatedPeriod(double& av, double& std) const
    {
        lock();
        if (estPIt == 0) {
            av = 0;
            std = 0;
        } else {
            av = totalT / estPIt;
            if (estPIt > 1) {
                std = sqrt(((1.0 / (estPIt - 1)) * (sumTSq - estPIt * av * av)));
            } else {
                std = 0;
            }
        }
        unlock();
    }

    unsigned int getIterations() const
    {
        lock();
        unsigned int ret = count;
        unlock();
        return ret;
    }

    double getEstimatedUsed() const
    {
        double ret;
        lock();
        if (count < 1) {
            ret = 0.0;
        } else {
            ret = totalUsed / count;
        }
        unlock();
        return ret;
    }

    void getEstimatedUsed(double& av, double& std) const
    {
        lock();
        if (count < 1) {
            av = 0;
            std = 0;
        } else {
            av = totalUsed / count;
            if (count > 1) {
                std = sqrt((1.0 / (count - 1)) * (sumUsedSq - count * av * av));
            } else {
                std = 0;
            }
        }
        unlock();
    }


    void step()
    {
        lock();
        currentRun = nowFunc();

        if (scheduleReset) {
            _resetStat();
        }

        if (count > 0) {
            double dT = (currentRun - previousRun);

            sumTSq += dT * dT;
            totalT += dT;

            if (adaptedPeriod < 0) {
                adaptedPeriod = 0;
            }

            estPIt++;
        }

        previousRun = currentRun;
        unlock();

        if (!suspended) {
            owner->run();
        }


        // At the end of each run of updateModule function, the thread is supposed
        // to be suspended and release CPU to other threads.
        // Calling a yield here will help the threads to alternate in the execution.
        // Note: call yield BEFORE computing elapsed time, so that any time spent due to
        // yield is took into account and the sleep time is correct.
        yield();

        lock();
        count++;
        double elapsed = nowFunc() - currentRun;
        //save last
        totalUsed += elapsed;
        sumUsedSq += elapsed * elapsed;
        unlock();

        sleepPeriod = adaptedPeriod - elapsed; // everything is in [seconds] except period, for it is used in the interface as [ms]

        delayFunc(sleepPeriod);
    }

    void run() override
    {
        while (!isClosing()) {
            step();
        }
    }

    bool threadInit() override
    {
        return owner->threadInit();
    }

    void threadRelease() override
    {
        owner->threadRelease();
    }

    bool setPeriod(double period)
    {
        adaptedPeriod = period;
        return true;
    }

    double getPeriod() const
    {
        return adaptedPeriod;
    }

    bool isSuspended() const
    {
        return suspended;
    }

    void suspend()
    {
        suspended = true;
    }

    void resume()
    {
        suspended = false;
    }

    void afterStart(bool s) override
    {
        owner->afterStart(s);
    }

    void beforeStart() override
    {
        owner->beforeStart();
    }

    void lock() const
    {
        mutex.lock();
    }

    void unlock() const
    {
        mutex.unlock();
    }
};


PeriodicThread::PeriodicThread(double period, ShouldUseSystemClock useSystemClock) :
        mPriv(new Private(this, period, useSystemClock))
{
}

PeriodicThread::~PeriodicThread()
{
    delete mPriv;
}

bool PeriodicThread::setPeriod(double period)
{
    return mPriv->setPeriod(period);
}

double PeriodicThread::getPeriod() const
{
    return mPriv->getPeriod();
}

bool PeriodicThread::isSuspended() const
{
    return mPriv->isSuspended();
}

void PeriodicThread::stop()
{
    mPriv->close();
}

void PeriodicThread::askToStop()
{
    mPriv->askToClose();
}

void PeriodicThread::step()
{
    mPriv->step();
}

bool PeriodicThread::start()
{
    return mPriv->start();
}

bool PeriodicThread::isRunning() const
{
    return mPriv->isRunning();
}

void PeriodicThread::suspend()
{
    mPriv->suspend();
}

void PeriodicThread::resume()
{
    mPriv->resume();
}

unsigned int PeriodicThread::getIterations() const
{
    return mPriv->getIterations();
}

double PeriodicThread::getEstimatedPeriod() const
{
    return mPriv->getEstimatedPeriod();
}

double PeriodicThread::getEstimatedUsed() const
{
    return mPriv->getEstimatedUsed();
}

void PeriodicThread::getEstimatedPeriod(double& av, double& std) const
{
    mPriv->getEstimatedPeriod(av, std);
}

void PeriodicThread::getEstimatedUsed(double& av, double& std) const
{
    mPriv->getEstimatedUsed(av, std);
}

void PeriodicThread::resetStat()
{
    mPriv->resetStat();
}

bool PeriodicThread::threadInit()
{
    return true;
}

void PeriodicThread::threadRelease()
{
}

void PeriodicThread::beforeStart()
{
}

void PeriodicThread::afterStart(bool success)
{
    YARP_UNUSED(success);
}

int PeriodicThread::setPriority(int priority, int policy)
{
    return mPriv->setPriority(priority, policy);
}

int PeriodicThread::getPriority() const
{
    return mPriv->getPriority();
}

int PeriodicThread::getPolicy() const
{
    return mPriv->getPolicy();
}
