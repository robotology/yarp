/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cstdio>
using namespace yarp::dev;

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

ReturnValue ImplementMotor::getNumberOfMotors(int *num)
{
    (*num)=castToMapper(helper)->axes();
    return ReturnValue_ok;
}

ReturnValue ImplementMotor::getTemperature(int m, double* value)
{
    MOTORIDCHECK(MAPPER_MAXID)
    ReturnValue ret;
    int k=castToMapper(helper)->toHw(m);

    ret=imotor->getTemperatureRaw(k, value);

    return ret;
}

ReturnValue ImplementMotor::getTemperatureLimit(int m, double* value)
{
    MOTORIDCHECK(MAPPER_MAXID)
    ReturnValue ret;
    int k=castToMapper(helper)->toHw(m);

    ret=imotor->getTemperatureLimitRaw(k, value);

    return ret;
}

ReturnValue ImplementMotor::setTemperatureLimit(int m, const double value)
{
    MOTORIDCHECK(MAPPER_MAXID)
    ReturnValue ret;
    int k=castToMapper(helper)->toHw(m);

    ret=imotor->setTemperatureLimitRaw(k, value);

    return ret;
}

ReturnValue ImplementMotor::getGearboxRatio(int m, double* value)
{
    MOTORIDCHECK(MAPPER_MAXID)
    ReturnValue ret;
    int k = castToMapper(helper)->toHw(m);

    ret = imotor->getGearboxRatioRaw(k, value);

    return ret;
}

ReturnValue ImplementMotor::setGearboxRatio(int m, const double value)
{
    MOTORIDCHECK(MAPPER_MAXID)
    ReturnValue ret;
    int k = castToMapper(helper)->toHw(m);

    ret = imotor->setGearboxRatioRaw(k, value);

    return ret;
}

ReturnValue ImplementMotor::getTemperatures(double *v)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    ReturnValue ret = imotor->getTemperaturesRaw(buffValues.getData());
    for (size_t i=0; i< buffValues.getSize(); i++)
    {
        int k = castToMapper(helper)->toHw(i);
        v[i] = buffValues[k];
    }
    return ret;
}
