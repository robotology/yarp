/*
* Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
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
    helper = 0;
}

bool ImplementPWMControl::initialize(int size, const int *amap, const double* dutyToPWM)
{
    if (helper != 0)
        return false;

    double *dummy = new double[size];
    for (int k = 0; k<size; k++)
        dummy[k] = 0;

    helper = (void *)(new ControlBoardHelper(size, amap, dummy, dummy, dummy, 0, 0, dutyToPWM));
    yAssert(helper != 0);

    delete[] dummy;
    return true;
}

ImplementPWMControl::~ImplementPWMControl()
{
    uninitialize();
}

bool ImplementPWMControl::uninitialize()
{
    if (helper != 0)
    {
        delete castToMapper(helper);
        helper = 0;
    }

    return true;
}

bool ImplementPWMControl::setRefDutyCycle(int j, double v)
{
    JOINTIDCHECK
        int k = castToMapper(helper)->toHw(j);

    return raw->setRefDutyCycleRaw(k, v);
}

bool ImplementPWMControl::setRefDutyCycles(const double *v)
{
    castToMapper(helper)->toHw(v, dummy);

    return raw->setRefDutyCyclesRaw(dummy);
}

bool ImplementPWMControl::getRefDutyCycle(int j, double *v)
{
    JOINTIDCHECK
        int k = castToMapper(helper)->toHw(j);

    bool ret = raw->getRefDutyCycleRaw(k, v);
    return ret;
}

bool ImplementPWMControl::getRefDutyCycles(double *v)
{
    bool ret = raw->getRefDutyCyclesRaw(dummy);

    castToMapper(helper)->toUser(dummy, v);
    return ret;
}

bool ImplementPWMControl::getDutyCycle(int j, double *v)
{
    JOINTIDCHECK
        int k = castToMapper(helper)->toHw(j);

    bool ret = raw->getDutyCycleRaw(k, v);
    return ret;
}

bool ImplementPWMControl::getDutyCycles(double *v)
{
    bool ret = raw->getDutyCyclesRaw(dummy);

    castToMapper(helper)->toUser(dummy, v);
    return ret;
}
