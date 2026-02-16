/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ImplementAmplifierControl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>

#include <cmath>

// Be careful: this file contains template implementations and is included by translation
// units that use the template (e.g. .cpp files). Avoid putting here non-template functions to
// avoid multiple definitions.

using namespace yarp::dev;

ImplementAmplifierControl::ImplementAmplifierControl(yarp::dev::IAmplifierControlRaw  *y)
{
    m_iraw= y;
    m_helper = nullptr;
}

ImplementAmplifierControl::~ImplementAmplifierControl()
{
    uninitialize();
}

bool ImplementAmplifierControl:: initialize (int size, const int *amap, const double *enc, const double *zos, const double *ampereFactor, const double *voltFactor)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,nullptr, ampereFactor, voltFactor));
    yAssert (m_helper != nullptr);

    m_buffer_ints.resize   (size);
    m_buffer_doubles.resize(size);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementAmplifierControl::uninitialize ()
{
    if (m_helper != nullptr) {
        delete castToMapper(m_helper);
    }

    m_helper=nullptr;
    return true;
}

ReturnValue ImplementAmplifierControl::enableAmp(int j)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k=castToMapper(m_helper)->toHw(j);

    return m_iraw->enableAmpRaw(k);
}

ReturnValue ImplementAmplifierControl::disableAmp(int j)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k=castToMapper(m_helper)->toHw(j);

    return m_iraw->disableAmpRaw(k);
}

ReturnValue ImplementAmplifierControl::getCurrents(double *currs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(currs)

    ReturnValue ret=m_iraw->getCurrentsRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->ampereS2A(m_buffer_doubles.data(), currs);
    return ret;
}

ReturnValue ImplementAmplifierControl::getCurrent(int j, double *c)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(c)

    double temp = 0;
    int k = castToMapper(m_helper)->toHw(j);
    ReturnValue ret = m_iraw->getCurrentRaw(k, &temp);
    castToMapper(m_helper)->ampereS2A(temp, k, *c, j);
    return ret;
}

ReturnValue ImplementAmplifierControl::getAmpStatus(int *st)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(st)

    ReturnValue ret=m_iraw->getAmpStatusRaw(m_buffer_ints.data());
    castToMapper(m_helper)->toUser(m_buffer_ints.data(), st);

    return ret;
}

ReturnValue ImplementAmplifierControl::getAmpStatus(int k, int *st)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(k)
    POINTERCHECK(st)

    int j=castToMapper(m_helper)->toHw(k);
    ReturnValue ret=m_iraw->getAmpStatusRaw(j, st);

    return ret;
}

ReturnValue ImplementAmplifierControl::setMaxCurrent(int m, double v)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)

    int k;
    double curr;
    castToMapper(m_helper)->ampereA2S(v, m, curr, k);
    return m_iraw->setMaxCurrentRaw(k, curr);
}

ReturnValue ImplementAmplifierControl::getMaxCurrent(int j, double* v)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(v)

    double val;
    int k=castToMapper(m_helper)->toHw(j);
    ReturnValue ret = m_iraw->getMaxCurrentRaw(k, &val);
    *v = castToMapper(m_helper)->ampereS2A(val, k);
    return ret;
}

ReturnValue ImplementAmplifierControl::getNominalCurrent(int m, double *curr)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(curr)

    int k;
    ReturnValue ret;
    double tmp;

    k=castToMapper(m_helper)->toHw(m);
    ret=m_iraw->getNominalCurrentRaw(k, &tmp);
    *curr=castToMapper(m_helper)->ampereS2A(tmp, k);
    return ret;
}

ReturnValue ImplementAmplifierControl::getPeakCurrent(int m, double *curr)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(curr)

    int k;
    ReturnValue ret;
    double tmp;

    k=castToMapper(m_helper)->toHw(m);
    ret=m_iraw->getPeakCurrentRaw(k, &tmp);
    *curr=castToMapper(m_helper)->ampereS2A(tmp, k);
    return ret;
}

ReturnValue ImplementAmplifierControl::setPeakCurrent(int m, const double curr)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)

    int k;
    double val;
    castToMapper(m_helper)->ampereA2S(curr, m, val, k);
    return m_iraw->setPeakCurrentRaw(k, val);
}

ReturnValue ImplementAmplifierControl::setNominalCurrent(int m, const double curr)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)

    int k;
    double val;
    castToMapper(m_helper)->ampereA2S(curr, m, val, k);
    return m_iraw->setNominalCurrentRaw(k, val);
}

ReturnValue ImplementAmplifierControl::getPWM(int m, double* pwm)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(pwm)

    int k;
    k=castToMapper(m_helper)->toHw(m);
    return m_iraw->getPWMRaw(k, pwm);
}

ReturnValue ImplementAmplifierControl::getPWMLimit(int m, double* limit)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(limit)

    int k;
    k=castToMapper(m_helper)->toHw(m);
    return m_iraw->getPWMLimitRaw(k, limit);
}

ReturnValue ImplementAmplifierControl::setPWMLimit(int m, const double limit)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)

    int k;
    k=castToMapper(m_helper)->toHw(m);
    return m_iraw->setPWMLimitRaw(k, limit);
}

ReturnValue ImplementAmplifierControl::getPowerSupplyVoltage(int m, double *voltage)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(voltage)

    int k;
    k=castToMapper(m_helper)->toHw(m);
    return m_iraw->getPowerSupplyVoltageRaw(k, voltage);
}
