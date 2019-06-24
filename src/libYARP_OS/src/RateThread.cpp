/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/RateThread.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0

RateThread::RateThread(int period) :
        PeriodicThread(period / 1000.0)
{
}

RateThread::~RateThread() = default;

bool RateThread::setRate(int period)
{
    return PeriodicThread::setPeriod(period / 1000.0);
}

double RateThread::getRate()
{
    return PeriodicThread::getPeriod() * 1000;
}

bool RateThread::isSuspended()
{
    return PeriodicThread::isSuspended();
}

void RateThread::stop()
{
    PeriodicThread::stop();
}

void RateThread::askToStop()
{
    PeriodicThread::askToStop();
}

bool RateThread::step()
{
    PeriodicThread::step();
    return true;
}

bool RateThread::start()
{
    return PeriodicThread::start();
}

bool RateThread::isRunning()
{
    return PeriodicThread::isRunning();
}

void RateThread::suspend()
{
    PeriodicThread::suspend();
}

void RateThread::resume()
{
    PeriodicThread::resume();
}

unsigned int RateThread::getIterations()
{
    return PeriodicThread::getIterations();
}

double RateThread::getEstPeriod()
{
    return PeriodicThread::getEstimatedPeriod() * 1000;
}

double RateThread::getEstUsed()
{
    return PeriodicThread::getEstimatedUsed() * 1000;
}

void RateThread::getEstPeriod(double& av, double& std)
{
    PeriodicThread::getEstimatedPeriod(av, std);
    av *= 1000;
    std *= 1000;
}

void RateThread::getEstUsed(double& av, double& std)
{
    PeriodicThread::getEstimatedUsed(av, std);
    av *= 1000;
    std *= 1000;
}

void RateThread::resetStat()
{
    PeriodicThread::resetStat();
}

bool RateThread::threadInit()
{
    return true;
}

void RateThread::threadRelease()
{
}

void RateThread::beforeStart()
{
}

void RateThread::afterStart(bool success)
{
    YARP_UNUSED(success);
}

int RateThread::setPriority(int priority, int policy)
{
    return PeriodicThread::setPriority(priority, policy);
}

int RateThread::getPriority()
{
    return PeriodicThread::getPriority();
}

int RateThread::getPolicy()
{
    return PeriodicThread::getPolicy();
}

//
//  System Rate Thread
//

SystemRateThread::SystemRateThread(int period) :
        PeriodicThread(period / 1000.0, ShouldUseSystemClock::Yes)
{
}

SystemRateThread::~SystemRateThread() = default;

bool SystemRateThread::stepSystem()
{
    step();
    return true;
}

#endif

RateThreadWrapper::RateThreadWrapper() :
        PeriodicThread(0)
{
    helper = nullptr;
    owned = false;
}


RateThreadWrapper::RateThreadWrapper(Runnable* helper) :
        PeriodicThread(0)
{
    this->helper = helper;
    owned = true;
}

RateThreadWrapper::RateThreadWrapper(Runnable& helper) :
        PeriodicThread(0)
{
    this->helper = &helper;
    owned = false;
}

RateThreadWrapper::~RateThreadWrapper()
{
    detach();
}

void RateThreadWrapper::detach()
{
    if (owned) {
        delete helper;
    }
    helper = nullptr;
    owned = false;
}

bool RateThreadWrapper::attach(Runnable& helper)
{
    detach();
    this->helper = &helper;
    owned = false;
    return true;
}

bool RateThreadWrapper::attach(Runnable* helper)
{
    detach();
    this->helper = helper;
    owned = true;
    return true;
}

bool RateThreadWrapper::open(double framerate, bool polling)
{
    double period = 0.0;
    if (framerate > 0) {
        period = (1.0 / framerate);
        YARP_SPRINTF2(yarp::os::impl::Logger::get(), info, "Setting framerate to: %.0lf[Hz] (thread period %f[s])\n", framerate, period);
    } else {
        YARP_SPRINTF0(yarp::os::impl::Logger::get(), info, "No framerate specified, polling the device");
        period = 0.0; //continuous
    }
    PeriodicThread::setPeriod(period);
    if (!polling) {
        start();
    }
    return true;
}

void RateThreadWrapper::close()
{
    PeriodicThread::stop();
}

void RateThreadWrapper::stop()
{
    PeriodicThread::stop();
}

void RateThreadWrapper::run()
{
    if (helper != nullptr) {
        helper->run();
    }
}

bool RateThreadWrapper::threadInit()
{
    if (helper != nullptr) {
        return helper->threadInit();
    }
    return true;
}

void RateThreadWrapper::threadRelease()
{
    if (helper != nullptr) {
        helper->threadRelease();
    }
}

void RateThreadWrapper::afterStart(bool success)
{
    if (helper != nullptr) {
        helper->afterStart(success);
    }
}

void RateThreadWrapper::beforeStart()
{
    if (helper != nullptr) {
        helper->beforeStart();
    }
}

Runnable* RateThreadWrapper::getAttachment() const
{
    return helper;
}
