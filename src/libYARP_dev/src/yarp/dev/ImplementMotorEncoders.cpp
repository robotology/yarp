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
ImplementMotorEncoders::ImplementMotorEncoders(IMotorEncodersRaw *y):
    m_iraw(y),
    m_helper(nullptr)
{;}

ImplementMotorEncoders::~ImplementMotorEncoders()
{
    uninitialize();
}

bool ImplementMotorEncoders:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert (m_helper != nullptr);

    m_buffer_ints.resize   (size);
    m_buffer_doubles.resize(size);
    m_buffer_doubles2.resize(size);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementMotorEncoders::uninitialize ()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

    return true;
}

ReturnValue ImplementMotorEncoders::getNumberOfMotorEncoders(int *num)
{
    std::lock_guard lock(m_imp_mutex);

    (*num)=castToMapper(m_helper)->axes();
    return ReturnValue_ok;
}

ReturnValue ImplementMotorEncoders::resetMotorEncoder(int m)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)

    int k;
    k=castToMapper(m_helper)->toHw(m);

    return m_iraw->resetMotorEncoderRaw(k);
}

ReturnValue ImplementMotorEncoders::resetMotorEncoders()
{
    std::lock_guard lock(m_imp_mutex);

    return m_iraw->resetMotorEncodersRaw();
}

ReturnValue ImplementMotorEncoders::setMotorEncoder(int m, const double val)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)

    int k;
    double enc;

    castToMapper(m_helper)->posA2E(val, m, enc, k);

    return m_iraw->setMotorEncoderRaw(k, enc);
}

ReturnValue ImplementMotorEncoders::getMotorEncoderCountsPerRevolution(int m, double* cpr)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(cpr)

    ReturnValue ret;
    int k=castToMapper(m_helper)->toHw(m);

    ret=m_iraw->getMotorEncoderCountsPerRevolutionRaw(k, cpr);

    return ret;
}

ReturnValue ImplementMotorEncoders::setMotorEncoderCountsPerRevolution(int m, double cpr)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)

    int k;

    k=castToMapper(m_helper)->toHw(m);

    return m_iraw->setMotorEncoderCountsPerRevolutionRaw(k, cpr);
}

ReturnValue ImplementMotorEncoders::setMotorEncoders(const double *val)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(val)

    castToMapper(m_helper)->posA2E(val, m_buffer_doubles.data());

    ReturnValue ret = m_iraw->setMotorEncodersRaw(m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementMotorEncoders::getMotorEncoder(int m, double *v)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(v)

    int k;
    double enc;
    ReturnValue ret;

    k=castToMapper(m_helper)->toHw(m);

    ret=m_iraw->getMotorEncoderRaw(k, &enc);

    *v=castToMapper(m_helper)->posE2A(enc, k);

    return ret;
}

ReturnValue ImplementMotorEncoders::getMotorEncoders(double *v)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(v)

    ReturnValue ret=m_iraw->getMotorEncodersRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->posE2A(m_buffer_doubles.data(), v);

    return ret;
}

ReturnValue ImplementMotorEncoders::getMotorEncoderSpeed(int m, double *v)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(v)

    int k;
    double enc;
    ReturnValue ret;

    k=castToMapper(m_helper)->toHw(m);

    ret=m_iraw->getMotorEncoderSpeedRaw(k, &enc);

    *v=castToMapper(m_helper)->velE2A(enc, k);

    return ret;
}

ReturnValue ImplementMotorEncoders::getMotorEncoderSpeeds(double *v)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(v)

    ReturnValue ret=m_iraw->getMotorEncoderSpeedsRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->velE2A(m_buffer_doubles.data(), v);

    return ret;
}

ReturnValue ImplementMotorEncoders::getMotorEncoderAcceleration(int m, double *v)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(v)

    int k;
    double enc;
    ReturnValue ret;

    k=castToMapper(m_helper)->toHw(m);

    ret=m_iraw->getMotorEncoderAccelerationRaw(k, &enc);

    *v=castToMapper(m_helper)->accE2A(enc, k);

    return ret;
}

ReturnValue ImplementMotorEncoders::getMotorEncoderAccelerations(double *v)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(v)

    ReturnValue ret=m_iraw->getMotorEncoderAccelerationsRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->accE2A(m_buffer_doubles.data(), v);

    return ret;
}

ReturnValue ImplementMotorEncoders::getMotorEncoderTimed(int m, double *v, double *t)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(m)
    POINTERCHECK(v)
    POINTERCHECK(t)

    int k;
    double enc;
    ReturnValue ret;

    k=castToMapper(m_helper)->toHw(m);

    ret=m_iraw->getMotorEncoderTimedRaw(k, &enc, t);

    *v=castToMapper(m_helper)->posE2A(enc, k);

    return ret;
}


ReturnValue ImplementMotorEncoders::getMotorEncodersTimed(double *v, double *t)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(v)
    POINTERCHECK(t)

    ReturnValue ret=m_iraw->getMotorEncodersTimedRaw(m_buffer_doubles.data(), m_buffer_doubles2.data());
    castToMapper(m_helper)->posE2A(m_buffer_doubles.data(), v);
    castToMapper(m_helper)->toUser(m_buffer_doubles2.data(), t);

    return ret;
}
