/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <cstdio>
using namespace yarp::dev;
#define JOINTIDCHECK if (m >= castToMapper(helper)->axes()){yError("motor id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

////////////////////////
// Encoder Interface Timed Implementation
ImplementMotor::ImplementMotor(IMotorRaw *y)
{
    imotor=y;
    helper = nullptr;
    temp1=nullptr;
    temp2=nullptr;
}

ImplementMotor::~ImplementMotor()
{
    uninitialize();
}

bool ImplementMotor:: initialize (int size, const int *amap)
{
    if (helper!=nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, nullptr, 0, 0));
    yAssert (helper != nullptr);
    temp1=new double [size];
    yAssert (temp1 != nullptr);
    temp2=new double [size];
    yAssert (temp2 != nullptr);
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementMotor::uninitialize ()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

    checkAndDestroy(temp1);
    checkAndDestroy(temp2);

    return true;
}

bool ImplementMotor::getNumberOfMotors(int *num)
{
    (*num)=castToMapper(helper)->axes();
    return true;
}

bool ImplementMotor::getTemperature(int m, double* value)
{
    JOINTIDCHECK
    bool ret;
    int k=castToMapper(helper)->toHw(m);

    ret=imotor->getTemperatureRaw(k, value);

    return ret;
}

bool ImplementMotor::getTemperatureLimit(int m, double* value)
{
    JOINTIDCHECK
    bool ret;
    int k=castToMapper(helper)->toHw(m);

    ret=imotor->getTemperatureLimitRaw(k, value);

    return ret;
}

bool ImplementMotor::setTemperatureLimit(int m, const double value)
{
    JOINTIDCHECK
    bool ret;
    int k=castToMapper(helper)->toHw(m);

    ret=imotor->setTemperatureLimitRaw(k, value);

    return ret;
}

bool ImplementMotor::getGearboxRatio(int m, double* value)
{
    JOINTIDCHECK
    bool ret;
    int k = castToMapper(helper)->toHw(m);

    ret = imotor->getGearboxRatioRaw(k, value);

    return ret;
}

bool ImplementMotor::setGearboxRatio(int m, const double value)
{
    JOINTIDCHECK
    bool ret;
    int k = castToMapper(helper)->toHw(m);

    ret = imotor->setGearboxRatioRaw(k, value);

    return ret;
}

bool ImplementMotor::getTemperatures(double *v)
{
    return imotor->getTemperaturesRaw(v);
}
