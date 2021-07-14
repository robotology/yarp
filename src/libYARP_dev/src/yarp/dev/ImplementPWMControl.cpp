/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ImplementPWMControl.h"
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>
#include <iostream>

using namespace yarp::dev;
using namespace yarp::os;

#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

/////////////// implement ImplemenPWMControl
ImplementPWMControl::ImplementPWMControl(IPWMControlRaw *r) :
    helper(nullptr),
    raw(r),
    doubleBuffManager(nullptr)
{;}

bool ImplementPWMControl::initialize(int size, const int *amap, const double* dutyToPWM)
{
    if (helper != nullptr) {
        return false;
    }

    helper = (void *)(new ControlBoardHelper(size, amap, nullptr, nullptr, nullptr, nullptr, nullptr, dutyToPWM));
    yAssert(helper != nullptr);

    doubleBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<double> (size);
    yAssert (doubleBuffManager != nullptr);

    return true;
}

ImplementPWMControl::~ImplementPWMControl()
{
    uninitialize();
}

bool ImplementPWMControl::uninitialize()
{
    if (helper != nullptr)
    {
        delete castToMapper(helper);
        helper = nullptr;
    }

    if(doubleBuffManager)
    {
        delete doubleBuffManager;
        doubleBuffManager=nullptr;
    }

    return true;
}

bool ImplementPWMControl::getNumberOfMotors(int *axes)
{
    return raw->getNumberOfMotorsRaw(axes);
}

bool ImplementPWMControl::setRefDutyCycle(int j, double duty)
{
    JOINTIDCHECK
    int k;
    double pwm;
    castToMapper(helper)->dutycycle2PWM(duty, j, pwm, k);
    return raw->setRefDutyCycleRaw(k, pwm);
}

bool ImplementPWMControl::setRefDutyCycles(const double *duty)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->dutycycle2PWM(duty, buffValues.getData());
    bool ret = raw->setRefDutyCyclesRaw( buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPWMControl::getRefDutyCycle(int j, double *v)
{
    JOINTIDCHECK
    double pwm;
    int k = castToMapper(helper)->toHw(j);
    bool ret = raw->getRefDutyCycleRaw(k, &pwm);
    *v = castToMapper(helper)->PWM2dutycycle(pwm, k);
    return ret;
}

bool ImplementPWMControl::getRefDutyCycles(double *duty)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret = raw->getRefDutyCyclesRaw(buffValues.getData());
    castToMapper(helper)->PWM2dutycycle(buffValues.getData(), duty);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPWMControl::getDutyCycle(int j, double *duty)
{
    JOINTIDCHECK
    double pwm;
    int k = castToMapper(helper)->toHw(j);
    bool ret = raw->getDutyCycleRaw(k, &pwm);
    *duty = castToMapper(helper)->PWM2dutycycle(pwm, k);
    return ret;
}

bool ImplementPWMControl::getDutyCycles(double *duty)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret = raw->getDutyCyclesRaw(buffValues.getData());
    castToMapper(helper)->PWM2dutycycle(buffValues.getData(), duty);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}
