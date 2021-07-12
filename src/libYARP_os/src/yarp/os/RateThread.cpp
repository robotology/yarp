/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#include <yarp/os/RateThread.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE

YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING

#include <yarp/os/impl/LogComponent.h>

using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(RATETHREAD, "yarp.os.RateThread" )
}


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
        yCInfo(RATETHREAD, "Setting framerate to: %.0lf[Hz] (thread period %f[s])\n", framerate, period);
    } else {
        yCInfo(RATETHREAD, "No framerate specified, polling the device");
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

YARP_WARNING_POP
