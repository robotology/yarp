/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ImplementEncoders.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>

#include <cmath>

// Be careful: this file contains template implementations and is included by translation
// units that use the template (e.g. .cpp files). Avoid putting here non-template functions to
// avoid multiple definitions.

using namespace yarp::dev;

////////////////////////
// Encoder Interface Implementation
ImplementEncoders::ImplementEncoders(yarp::dev::IEncodersRaw  *y)
{
    m_iraw= y;
    m_helper = nullptr;
}

ImplementEncoders::~ImplementEncoders()
{
    uninitialize();
}

bool ImplementEncoders:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert (m_helper != nullptr);

    m_buffer_doubles.resize(size);
    m_buffer_ints.resize(size);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementEncoders::uninitialize ()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

    return true;
}

ReturnValue ImplementEncoders::getAxes(int *ax)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(ax)

    (*ax)=castToMapper(m_helper)->axes();
    return ReturnValue_ok;
}

ReturnValue ImplementEncoders::resetEncoder(int j)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    k=castToMapper(m_helper)->toHw(j);

    return m_iraw->resetEncoderRaw(k);
}

ReturnValue ImplementEncoders::resetEncoders()
{
    std::lock_guard lock(m_imp_mutex);

    return m_iraw->resetEncodersRaw();
}

ReturnValue ImplementEncoders::setEncoder(int j, double val)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double enc;

    castToMapper(m_helper)->posA2E(val, j, enc, k);

    return m_iraw->setEncoderRaw(k, enc);
}

ReturnValue ImplementEncoders::setEncoders(const double *val)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(val)

    castToMapper(m_helper)->posA2E(val, m_buffer_doubles.data());

    return m_iraw->setEncodersRaw(m_buffer_doubles.data());
}

ReturnValue ImplementEncoders::getEncoder(int j, double *v)
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

ReturnValue ImplementEncoders::getEncoders(double *v)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(v)

    ReturnValue ret;
    castToMapper(m_helper)->axes();

    ret=m_iraw->getEncodersRaw(m_buffer_doubles.data());

    castToMapper(m_helper)->posE2A(m_buffer_doubles.data(), v);

    return ret;
}

ReturnValue ImplementEncoders::getEncoderSpeed(int j, double *v)
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

ReturnValue ImplementEncoders::getEncoderSpeeds(double *v)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(v)

    ReturnValue ret;
    ret=m_iraw->getEncoderSpeedsRaw(m_buffer_doubles.data());

    castToMapper(m_helper)->velE2A(m_buffer_doubles.data(), v);

    return ret;
}

ReturnValue ImplementEncoders::getEncoderAcceleration(int j, double *v)
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

ReturnValue ImplementEncoders::getEncoderAccelerations(double *v)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(v)

    ReturnValue ret;
    ret=m_iraw->getEncoderAccelerationsRaw(m_buffer_doubles.data());

    castToMapper(m_helper)->accE2A(m_buffer_doubles.data(), v);

    return ret;
}
