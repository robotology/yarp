/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ImplementControlLimits.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>

using namespace yarp::dev;


ImplementControlLimits::ImplementControlLimits(yarp::dev::IControlLimitsRaw *y) :
    m_iraw(y),
        m_helper(nullptr)
{;}


ImplementControlLimits::~ImplementControlLimits()
{
    uninitialize();
}

/**
 * Clean up internal data and memory.
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementControlLimits::uninitialize()
{
    if(m_helper != nullptr)
    {
        delete castToMapper(m_helper);
        m_helper = nullptr;
    }
    return true;
}

bool ImplementControlLimits::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert(m_helper != nullptr);

    m_buffer_ints.resize   (size);
    m_buffer_doubles.resize(size);

    return true;
}


ReturnValue ImplementControlLimits::setPosLimits(int axis, double min, double max)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(axis);

    double minEnc=0;
    double maxEnc=0;

    int k=0;
    castToMapper(m_helper)->posA2E(min, axis, minEnc, k);
    castToMapper(m_helper)->posA2E(max, axis, maxEnc, k);

    if( (max > min) && (minEnc > maxEnc)) //angle to encoder conversion factor is negative
    {
        double temp;   // exchange max and min limits
        temp = minEnc;
        minEnc = maxEnc;
        maxEnc = temp;
    }

    return m_iraw->setPosLimitsRaw(k, minEnc, maxEnc);
}


ReturnValue ImplementControlLimits::getPosLimits(int axis, double *min, double *max)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(axis)
    POINTERCHECK(min)
    POINTERCHECK(max)

    double minEnc=0;
    double maxEnc=0;

    int k=castToMapper(m_helper)->toHw(axis);
    ReturnValue ret=m_iraw->getPosLimitsRaw(k, &minEnc, &maxEnc);

    *min=castToMapper(m_helper)->posE2A(minEnc, k);
    *max=castToMapper(m_helper)->posE2A(maxEnc, k);

    if( (*max < *min) && (minEnc < maxEnc)) //angle to encoder conversion factor is negative
    {
        double temp;   // exchange max and min limits
        temp = *min;
        *min = *max;
        *max = temp;
    }
    return ret;
}

ReturnValue ImplementControlLimits::setVelLimits(int axis, double min, double max)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(axis);

    double minEnc=0;
    double maxEnc=0;

    int k=0;
    castToMapper(m_helper)->velA2E_abs(min, axis, minEnc, k);
    castToMapper(m_helper)->velA2E_abs(max, axis, maxEnc, k);

    return m_iraw->setVelLimitsRaw(k, minEnc, maxEnc);
}

ReturnValue ImplementControlLimits::getVelLimits(int axis, double *min, double *max)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(axis);
    POINTERCHECK(min)
    POINTERCHECK(max)

    double minEnc=0;
    double maxEnc=0;

    int k=castToMapper(m_helper)->toHw(axis);
    ReturnValue ret=m_iraw->getVelLimitsRaw(k, &minEnc, &maxEnc);

    *min = castToMapper(m_helper)->velE2A_abs(minEnc, k);
    *max = castToMapper(m_helper)->velE2A_abs(maxEnc, k);

    return ret;
}
