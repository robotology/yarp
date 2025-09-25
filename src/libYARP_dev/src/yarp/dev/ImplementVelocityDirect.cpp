/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ImplementVelocityDirect.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/os/Log.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

using namespace yarp::dev;
using namespace yarp::os;

ImplementVelocityDirect::ImplementVelocityDirect(IVelocityDirectRaw* y) :
    m_iVelocityDirectRaw(y)
{}

ImplementVelocityDirect::~ImplementVelocityDirect()
{
    uninitialize();
}

bool ImplementVelocityDirect::initialize(int size, const int* axis_map, const double* enc, const double* zeros)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper = (void*)(new ControlBoardHelper(size, axis_map, enc, zeros));
    yAssert(m_helper != nullptr);

    m_buffer_ints.resize(size);
    m_buffer_doubles.resize(size);

    return true;
}

bool ImplementVelocityDirect::uninitialize()
{
    if (m_helper != nullptr)
    {
        delete castToMapper(m_helper);
        m_helper = nullptr;
    }

    return true;
}

yarp::dev::ReturnValue ImplementVelocityDirect::getAxes(size_t& axes)
{
    axes = castToMapper(m_helper)->axes();
    return ReturnValue_ok;
}

yarp::dev::ReturnValue ImplementVelocityDirect::setRefVelocity(int j, double sp)
{
    if (j >= castToMapper(m_helper)->axes()) {
        yError("joint id out of bound");
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    int k;
    double enc;
    castToMapper(m_helper)->velA2E(sp, j, enc, k);
    return m_iVelocityDirectRaw->setRefVelocityRaw(k, enc);
}

yarp::dev::ReturnValue ImplementVelocityDirect::setRefVelocity(const std::vector<double>& vels)
{
    size_t axes = castToMapper(m_helper)->axes();
    if (vels.size() != axes) {
        yError("Input vector size does not match number of axes");
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    castToMapper(m_helper)->velA2E(vels.data(), m_buffer_doubles.data());
    auto ret = m_iVelocityDirectRaw->setRefVelocityRaw(m_buffer_doubles);
    return ret;
}

yarp::dev::ReturnValue ImplementVelocityDirect::setRefVelocity(const std::vector<int>& jnts, const std::vector<double>& vels)
{
    if (jnts.size() != vels.size()) {
        yError("Joints and velocities vectors must have the same size");
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    if (!castToMapper(m_helper)->checkAxesIds(static_cast<int>(jnts.size()), jnts.data())) {
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    for (size_t idx = 0; idx < jnts.size(); idx++) {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(jnts[idx]);
        m_buffer_doubles[idx] = castToMapper(m_helper)->velA2E(vels[idx], jnts[idx]);
    }

    auto ret = m_iVelocityDirectRaw->setRefVelocityRaw(m_buffer_ints, m_buffer_doubles);
    return ret;
}

yarp::dev::ReturnValue ImplementVelocityDirect::getRefVelocity(const int j, double& vel)
{
    if (j >= castToMapper(m_helper)->axes()) {
        yError("joint id out of bound");
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    int k = castToMapper(m_helper)->toHw(j);
    double tmp;
    auto ret = m_iVelocityDirectRaw->getRefVelocityRaw(k, tmp);
    vel = castToMapper(m_helper)->velE2A(tmp, k);
    return ret;
}

yarp::dev::ReturnValue ImplementVelocityDirect::getRefVelocity(std::vector<double>& vels)
{
    size_t axes = castToMapper(m_helper)->axes();
    auto ret = m_iVelocityDirectRaw->getRefVelocityRaw(m_buffer_doubles);
    if (vels.size() != axes) { vels.resize(axes); }
    castToMapper(m_helper)->velE2A(m_buffer_doubles.data(), vels.data());
    return ret;
}

yarp::dev::ReturnValue ImplementVelocityDirect::getRefVelocity(const std::vector<int>& jnts, std::vector<double>& vels)
{
    if (jnts.size() != vels.size()) {
        vels.resize(jnts.size());
    }
    if (!castToMapper(m_helper)->checkAxesIds(static_cast<int>(jnts.size()), jnts.data())) {
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    for (size_t idx = 0; idx < jnts.size(); idx++) {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(jnts[idx]);
    }

    auto ret = m_iVelocityDirectRaw->getRefVelocityRaw(m_buffer_ints, m_buffer_doubles);

    for (size_t idx = 0; idx < jnts.size(); idx++) {
        vels[idx] = castToMapper(m_helper)->velE2A(m_buffer_doubles[idx], jnts[idx]);
    }

    return ret;
}
