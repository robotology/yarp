/*
* Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
* Authors: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include "yarp/dev/ImplementPWMControl.h"
#include <yarp/dev/ControlBoardHelper.h>
#include <iostream>

using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define PJOINTIDCHECK(j) if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

/////////////// implement ImplemenPWMControl
ImplementPWMControl::ImplementPWMControl(IPWMControlRaw *r)
{
    raw = r;
    helper = nullptr;
}

bool ImplementPWMControl::initialize(int size, const int *amap, const double* dutyToPWM)
{
    if (helper != nullptr)
        return false;

    double *dummy = new double[size];
    for (int k = 0; k<size; k++)
        dummy[k] = 0;

    helper = (void *)(new ControlBoardHelper(size, amap, dummy, dummy, dummy, nullptr, 0, dutyToPWM));
    yAssert(helper != nullptr);

    delete[] dummy;
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
    castToMapper(helper)->dutycycle2PWM(duty, dummy);
    return raw->setRefDutyCyclesRaw(dummy);
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
    bool ret;
    ret = raw->getRefDutyCyclesRaw(dummy);
    castToMapper(helper)->PWM2dutycycle(dummy, duty);
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
    bool ret;
    ret = raw->getDutyCyclesRaw(dummy);
    castToMapper(helper)->PWM2dutycycle(dummy, duty);
    return ret;
}
