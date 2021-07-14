/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cstdio>
using namespace yarp::dev;

#define JOINTIDCHECK if (m >= castToMapper(helper)->axes()){yError("motor id out of bound"); return false;}

////////////////////////
// Encoder Interface Timed Implementation
ImplementMotor::ImplementMotor(IMotorRaw *y) :
    imotor(y),
    helper(nullptr),
    doubleBuffManager(nullptr)
{ }

ImplementMotor::~ImplementMotor()
{
    uninitialize();
}

bool ImplementMotor:: initialize (int size, const int *amap)
{
    if (helper != nullptr) {
        return false;
    }

    helper=(void *)(new ControlBoardHelper(size, amap));
    yAssert (helper != nullptr);

    doubleBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<double> (size);
    yAssert (doubleBuffManager != nullptr);

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

    if(doubleBuffManager)
    {
        delete doubleBuffManager;
        doubleBuffManager=nullptr;
    }

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
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    bool ret = imotor->getTemperaturesRaw(buffValues.getData());
    for (size_t i=0; i< buffValues.getSize(); i++)
    {
        int k = castToMapper(helper)->toHw(i);
        v[i] = buffValues[k];
    }
    return ret;
}
