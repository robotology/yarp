/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ImplementPWMControl.h"
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>
#include <iostream>

using namespace yarp::dev;
using namespace yarp::os;

/////////////// implement ImplemenPWMControl
ImplementPWMControl::ImplementPWMControl(IPWMControlRaw *r) :
    m_helper(nullptr),
    m_iraw(r)
{;}

bool ImplementPWMControl::initialize(int size, const int *amap, const double* dutyToPWM)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper = (void *)(new ControlBoardHelper(size, amap, nullptr, nullptr, nullptr, nullptr, nullptr, dutyToPWM));
    yAssert(m_helper != nullptr);

    m_buffer_ints.resize   (size);
    m_buffer_doubles.resize(size);

    return true;
}

ImplementPWMControl::~ImplementPWMControl()
{
    uninitialize();
}

bool ImplementPWMControl::uninitialize()
{
    if (m_helper != nullptr)
    {
        delete castToMapper(m_helper);
        m_helper = nullptr;
    }

    return true;
}

ReturnValue ImplementPWMControl::getNumberOfMotors(int *axes)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(axes);

    return m_iraw->getNumberOfMotorsRaw(axes);
}

ReturnValue ImplementPWMControl::setRefDutyCycle(int j, double duty)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double pwm;
    castToMapper(m_helper)->dutycycle2PWM(duty, j, pwm, k);
    return m_iraw->setRefDutyCycleRaw(k, pwm);
}

ReturnValue ImplementPWMControl::setRefDutyCycles(const double *duty)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(duty);

    castToMapper(m_helper)->dutycycle2PWM(duty, m_buffer_doubles.data());
    ReturnValue ret = m_iraw->setRefDutyCyclesRaw( m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPWMControl::getRefDutyCycle(int j, double *v)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(v);

    double pwm;
    int k = castToMapper(m_helper)->toHw(j);
    ReturnValue ret = m_iraw->getRefDutyCycleRaw(k, &pwm);
    *v = castToMapper(m_helper)->PWM2dutycycle(pwm, k);
    return ret;
}

ReturnValue ImplementPWMControl::getRefDutyCycles(double *duty)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(duty);

    ReturnValue ret = m_iraw->getRefDutyCyclesRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->PWM2dutycycle(m_buffer_doubles.data(), duty);

    return ret;
}

ReturnValue ImplementPWMControl::getDutyCycle(int j, double *duty)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(duty);

    double pwm;
    int k = castToMapper(m_helper)->toHw(j);
    ReturnValue ret = m_iraw->getDutyCycleRaw(k, &pwm);
    *duty = castToMapper(m_helper)->PWM2dutycycle(pwm, k);
    return ret;
}

ReturnValue ImplementPWMControl::getDutyCycles(double *duty)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(duty);

    ReturnValue ret = m_iraw->getDutyCyclesRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->PWM2dutycycle(m_buffer_doubles.data(), duty);

    return ret;
}
