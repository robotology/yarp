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

////////////////////////
// Encoder Interface Timed Implementation
ImplementEncodersTimed::ImplementEncodersTimed(IEncodersTimedRaw *y):
    m_iraw(y),
    m_helper(nullptr)
{;}

ImplementEncodersTimed::~ImplementEncodersTimed()
{
    uninitialize();
}

bool ImplementEncodersTimed:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert (m_helper != nullptr);

    m_buffer_doubles.resize(size);
    m_buffer_doubles2.resize(size);
    m_buffer_ints.resize(size);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementEncodersTimed::uninitialize ()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

    return true;
}

ReturnValue ImplementEncodersTimed::getAxes(int *ax)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(ax)

    (*ax)=castToMapper(m_helper)->axes();
    return ReturnValue_ok;
}

ReturnValue ImplementEncodersTimed::resetEncoder(int j)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    k=castToMapper(m_helper)->toHw(j);

    return m_iraw->resetEncoderRaw(k);
}

ReturnValue ImplementEncodersTimed::resetEncoders()
{
    std::lock_guard lock(m_imp_mutex);

    return m_iraw->resetEncodersRaw();
}

ReturnValue ImplementEncodersTimed::setEncoder(int j, double val)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double enc;

    castToMapper(m_helper)->posA2E(val, j, enc, k);

    return m_iraw->setEncoderRaw(k, enc);
}

ReturnValue ImplementEncodersTimed::setEncoders(const double *val)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(val)

    castToMapper(m_helper)->posA2E(val, m_buffer_doubles.data());
    ReturnValue ret = m_iraw->setEncodersRaw(m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementEncodersTimed::getEncoder(int j, double *v)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(v)

    int k;
    double enc;
    ReturnValue ret;

    k=castToMapper(m_helper)->toHw(j);

    ret=m_iraw->getEncoderRaw(k, &enc);

    *v=castToMapper(m_helper)->posE2A(enc, k);

    return ret;
}

ReturnValue ImplementEncodersTimed::getEncoders(double *v)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(v)

    ReturnValue ret = m_iraw->getEncodersRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->posE2A(m_buffer_doubles.data(), v);

    return ret;
}

ReturnValue ImplementEncodersTimed::getEncoderSpeed(int j, double *v)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(v)

    int k;
    double enc;
    ReturnValue ret;

    k=castToMapper(m_helper)->toHw(j);

    ret=m_iraw->getEncoderSpeedRaw(k, &enc);

    *v=castToMapper(m_helper)->velE2A(enc, k);

    return ret;
}

ReturnValue ImplementEncodersTimed::getEncoderSpeeds(double *v)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(v)

    ReturnValue ret=m_iraw->getEncoderSpeedsRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->velE2A(m_buffer_doubles.data(), v);

    return ret;
}

ReturnValue ImplementEncodersTimed::getEncoderAcceleration(int j, double *v)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(v)

    int k;
    double enc;
    ReturnValue ret;

    k=castToMapper(m_helper)->toHw(j);

    ret=m_iraw->getEncoderAccelerationRaw(k, &enc);

    *v=castToMapper(m_helper)->accE2A(enc, k);

    return ret;
}

ReturnValue ImplementEncodersTimed::getEncoderAccelerations(double *v)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(v)

    ReturnValue ret = m_iraw->getEncoderAccelerationsRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->accE2A(m_buffer_doubles.data(), v);

    return ret;
}

ReturnValue ImplementEncodersTimed::getEncoderTimed(int j, double *v, double *t)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(v)
    POINTERCHECK(t)

    int k;
    double enc;
    ReturnValue ret;

    k=castToMapper(m_helper)->toHw(j);

    ret=m_iraw->getEncoderTimedRaw(k, &enc, t);

    *v=castToMapper(m_helper)->posE2A(enc, k);

    return ret;
}


ReturnValue ImplementEncodersTimed::getEncodersTimed(double *v, double *t)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(v)
    POINTERCHECK(t)

    ReturnValue ret=m_iraw->getEncodersTimedRaw(m_buffer_doubles.data(), m_buffer_doubles2.data());

    castToMapper(m_helper)->posE2A(m_buffer_doubles.data(), v);
    castToMapper(m_helper)->toUser(m_buffer_doubles2.data(), t);

    return ret;
}
