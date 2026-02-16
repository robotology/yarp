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
using namespace yarp::os;

ImplementCurrentControl::ImplementCurrentControl(ICurrentControlRaw *tq):
    m_iraw(tq),
    m_helper(nullptr)
{;}

ImplementCurrentControl::~ImplementCurrentControl()
{
    uninitialize();
}

bool ImplementCurrentControl::initialize(int size, const int *amap, const double* ampsToSens)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper = (void *)(new ControlBoardHelper(size, amap, nullptr, nullptr, nullptr, ampsToSens, nullptr, nullptr));
    yAssert (m_helper != nullptr);

    m_buffer_doubles.resize(size);
    m_buffer_doubles2.resize(size);
    m_buffer_ints.resize(size);

    return true;
}

bool ImplementCurrentControl::uninitialize()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

    return true;
}

ReturnValue  ImplementCurrentControl::getNumberOfMotors(int *axes)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(axes)

    return m_iraw->getNumberOfMotorsRaw(axes);
}

ReturnValue ImplementCurrentControl::getRefCurrent(int j, double *r)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(r)

    int k;
    ReturnValue ret;
    double current;
    k=castToMapper(m_helper)->toHw(j);
    ret = m_iraw->getRefCurrentRaw(k, &current);
    *r = castToMapper(m_helper)->ampereS2A(current, k);
    return ret;
}

ReturnValue ImplementCurrentControl::getRefCurrents(double *t)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(t)

    ReturnValue ret = m_iraw->getRefCurrentsRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->ampereS2A(m_buffer_doubles.data(),t);

    return ret;
}

ReturnValue ImplementCurrentControl::setRefCurrents(const double *t)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(t)

    castToMapper(m_helper)->ampereA2S(t, m_buffer_doubles.data());
    ReturnValue ret = m_iraw->setRefCurrentsRaw(m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementCurrentControl::setRefCurrent(int j, double t)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double sens;
    castToMapper(m_helper)->ampereA2S(t,j,sens,k);
    return m_iraw->setRefCurrentRaw(k, sens);
}

ReturnValue ImplementCurrentControl::getCurrents(double *t)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(t)

    ReturnValue ret = m_iraw->getCurrentsRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->ampereS2A(m_buffer_doubles.data(), t);

    return ret;
}

ReturnValue ImplementCurrentControl::setRefCurrents(const int n_joints, const int *joints, const double *t)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints)
    POINTERCHECK(t)
    JOINTSIDCHECK(n_joints, joints)

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(joints[idx]);
        m_buffer_doubles[idx] = castToMapper(m_helper)->ampereA2S(t[idx], joints[idx]);
    }

    ReturnValue ret = m_iraw->setRefCurrentsRaw(n_joints, m_buffer_ints.data(), m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementCurrentControl::getCurrent(int j, double *t)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(t)

    int k;
    ReturnValue ret;
    double current;
    k=castToMapper(m_helper)->toHw(j);
    ret = m_iraw->getCurrentRaw(k, &current);
    *t = castToMapper(m_helper)->ampereS2A(current, k);
    return ret;
}

ReturnValue ImplementCurrentControl::getCurrentRanges(double *min, double *max)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(min)
    POINTERCHECK(max)

    ReturnValue ret = m_iraw->getCurrentRangesRaw(m_buffer_doubles.data(), m_buffer_doubles2.data());
    castToMapper(m_helper)->ampereS2A(m_buffer_doubles.data(), min);
    castToMapper(m_helper)->ampereS2A(m_buffer_doubles2.data(), max);

    return ret;
}

ReturnValue ImplementCurrentControl::getCurrentRange(int j, double *min, double *max)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(min)
    POINTERCHECK(max)

    int k;
    k=castToMapper(m_helper)->toHw(j);
    double min_t, max_t;
    ReturnValue ret = m_iraw->getCurrentRangeRaw(k, &min_t, &max_t);
    *min = castToMapper(m_helper)->ampereS2A(min_t, k);
    *max = castToMapper(m_helper)->ampereS2A(max_t, k);
    return ret;
}
