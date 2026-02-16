/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>

#include <cstdio>
using namespace yarp::dev;

////////////////////////
// Encoder Interface Timed Implementation
ImplementMotor::ImplementMotor(IMotorRaw *y) :
    m_iraw(y),
    m_helper(nullptr)
{ }

ImplementMotor::~ImplementMotor()
{
    uninitialize();
}

bool ImplementMotor:: initialize (int size, const int *amap)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap));
    yAssert (m_helper != nullptr);

    m_buffer_ints.resize   (size);
    m_buffer_doubles.resize(size);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementMotor::uninitialize ()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

    return true;
}

ReturnValue ImplementMotor::getNumberOfMotors(int *num)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(num)

    (*num)=castToMapper(m_helper)->axes();
    return ReturnValue_ok;
}

ReturnValue ImplementMotor::getTemperature(int m, double* value)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(value)

    ReturnValue ret;
    int k=castToMapper(m_helper)->toHw(m);

    ret=m_iraw->getTemperatureRaw(k, value);

    return ret;
}

ReturnValue ImplementMotor::getTemperatureLimit(int m, double* value)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(value)

    ReturnValue ret;
    int k=castToMapper(m_helper)->toHw(m);

    ret=m_iraw->getTemperatureLimitRaw(k, value);

    return ret;
}

ReturnValue ImplementMotor::setTemperatureLimit(int m, const double value)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)

    ReturnValue ret;
    int k=castToMapper(m_helper)->toHw(m);

    ret=m_iraw->setTemperatureLimitRaw(k, value);

    return ret;
}

ReturnValue ImplementMotor::getGearboxRatio(int m, double* value)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(value)

    ReturnValue ret;
    int k = castToMapper(m_helper)->toHw(m);

    ret = m_iraw->getGearboxRatioRaw(k, value);

    return ret;
}

ReturnValue ImplementMotor::setGearboxRatio(int m, const double value)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)

    ReturnValue ret;
    int k = castToMapper(m_helper)->toHw(m);

    ret = m_iraw->setGearboxRatioRaw(k, value);

    return ret;
}

ReturnValue ImplementMotor::getTemperatures(double *v)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(v)

    ReturnValue ret = m_iraw->getTemperaturesRaw(m_buffer_doubles.data());
    for (size_t i=0; i< castToMapper(m_helper)->axes(); i++)
    {
        int k = castToMapper(m_helper)->toHw(i);
        v[i] = m_buffer_doubles[k];
    }
    return ret;
}
