/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ImplementVelocityDirect.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
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
    std::lock_guard lock(m_imp_mutex);

    axes = castToMapper(m_helper)->axes();
    return ReturnValue_ok;
}

yarp::dev::ReturnValue ImplementVelocityDirect::setRefVelocity(int j, double sp)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double enc;
    castToMapper(m_helper)->velA2E(sp, j, enc, k);
    return m_iVelocityDirectRaw->setRefVelocityRaw(k, enc);
}

yarp::dev::ReturnValue ImplementVelocityDirect::setRefVelocity(const std::vector<double>& vels)
{
    std::lock_guard lock(m_imp_mutex);
    VECCHECK_SET_ALL(vels)

    castToMapper(m_helper)->velA2E(vels.data(), m_buffer_doubles.data());
    auto ret = m_iVelocityDirectRaw->setRefVelocityRaw(m_buffer_doubles);
    return ret;
}

yarp::dev::ReturnValue ImplementVelocityDirect::setRefVelocity(const std::vector<int>& joints, const std::vector<double>& vels)
{
    std::lock_guard lock(m_imp_mutex);
    VECCHECK_SET_SOME(joints, vels)

    std::vector<int> vectorInt_tmp(joints.size());
    std::vector<double> vectorDouble_tmp(joints.size());

    for (size_t idx = 0; idx < joints.size(); idx++) {
        vectorInt_tmp[idx] = castToMapper(m_helper)->toHw(joints[idx]);
        vectorDouble_tmp[idx] = castToMapper(m_helper)->velA2E(vels[idx], joints[idx]);
    }

    auto ret = m_iVelocityDirectRaw->setRefVelocityRaw(vectorInt_tmp, vectorDouble_tmp);
    return ret;
}

yarp::dev::ReturnValue ImplementVelocityDirect::getRefVelocity(const int j, double& vel)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k = castToMapper(m_helper)->toHw(j);
    double tmp;
    auto ret = m_iVelocityDirectRaw->getRefVelocityRaw(k, tmp);
    vel = castToMapper(m_helper)->velE2A(tmp, k);
    return ret;
}

yarp::dev::ReturnValue ImplementVelocityDirect::getRefVelocity(std::vector<double>& vels)
{
    std::lock_guard lock(m_imp_mutex);
    VECCHECK_GET_ALL(vels)

    size_t axes = castToMapper(m_helper)->axes();
    auto ret = m_iVelocityDirectRaw->getRefVelocityRaw(m_buffer_doubles);
    castToMapper(m_helper)->velE2A(m_buffer_doubles.data(), vels.data());
    return ret;
}

yarp::dev::ReturnValue ImplementVelocityDirect::getRefVelocity(const std::vector<int>& joints, std::vector<double>& vels)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTSIDVECCHECK(joints)
    VECCHECK_GET_SOME(joints, vels)

    std::vector<int> vectorInt_tmp(joints.size());
    std::vector<double> vectorDouble_tmp(joints.size());

    for (size_t idx = 0; idx < joints.size(); idx++) {
        vectorInt_tmp[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }

    auto ret = m_iVelocityDirectRaw->getRefVelocityRaw(vectorInt_tmp, vectorDouble_tmp);

    for (size_t idx = 0; idx < joints.size(); idx++) {
        vels[idx] = castToMapper(m_helper)->velE2A(vectorDouble_tmp[idx], joints[idx]);
    }

    return ret;
}
