/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ImplementControlMode.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cstdio>
using namespace yarp::dev;
using namespace yarp::os;

ImplementControlMode::ImplementControlMode(IControlModeRaw *r):
    m_helper(nullptr),
    m_iraw(r)
{}

bool ImplementControlMode::initialize(int size, const int *amap)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap));
    yAssert (m_helper != nullptr);

    yAssert(size > 0);
    m_vectorInt_tmp.resize(size);
    m_vectorCM_tmp.resize(size);
    m_vectorSCM_tmp.resize(size);

    return true;
}

ImplementControlMode::~ImplementControlMode()
{
    uninitialize();
}

bool ImplementControlMode::uninitialize ()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

    return true;
}

ReturnValue ImplementControlMode::getAvailableControlModes(int j, std::vector<yarp::dev::SelectableControlModeEnum>& avail)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k=castToMapper(m_helper)->toHw(j);
    return m_iraw->getAvailableControlModesRaw(k, avail);
}

ReturnValue ImplementControlMode::getControlMode(int j, yarp::dev::ControlModeEnum& mode)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k=castToMapper(m_helper)->toHw(j);
    return m_iraw->getControlModeRaw(k, mode);
}

ReturnValue ImplementControlMode::getControlModes(std::vector<yarp::dev::ControlModeEnum>& modes)
{
    std::lock_guard lock(m_imp_mutex);
    VECCHECK_GET_ALL(modes)

    ReturnValue ret=m_iraw->getControlModesRaw(m_vectorCM_tmp);

    for(int idx=0; idx<castToMapper(m_helper)->axes(); idx++)
    {
        modes[idx] = m_vectorCM_tmp[castToMapper(m_helper)->toHw(idx)];
    }

    return ret;
}

ReturnValue ImplementControlMode::getControlModes(std::vector<int> joints, std::vector<yarp::dev::ControlModeEnum>& modes)
{
    std::lock_guard lock(m_imp_mutex);
    VECCHECK_GET_SOME(joints,modes);

    std::vector<int> vectorInt_tmp(joints.size());
    for(int idx=0; idx<joints.size(); idx++)
    {
        vectorInt_tmp[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }
    ReturnValue ret = m_iraw->getControlModesRaw(vectorInt_tmp, modes);

    return ret;
}

ReturnValue ImplementControlMode::setControlMode(int j, yarp::dev::SelectableControlModeEnum mode)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k=castToMapper(m_helper)->toHw(j);
    return m_iraw->setControlModeRaw(k, mode);
}

ReturnValue ImplementControlMode::setControlModes(std::vector<int> joints, std::vector<yarp::dev::SelectableControlModeEnum> modes)
{
    std::lock_guard lock(m_imp_mutex);
    VECCHECK_SET_SOME(joints, modes)

    std::vector<int> vectorInt_tmp(joints.size());
    for(int idx=0; idx<joints.size(); idx++)
    {
        vectorInt_tmp[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }

    ReturnValue ret = m_iraw->setControlModesRaw(vectorInt_tmp, modes);

    return ret;
}

ReturnValue ImplementControlMode::setControlModes(std::vector<yarp::dev::SelectableControlModeEnum> modes)
{
    std::lock_guard lock(m_imp_mutex);
    VECCHECK_SET_ALL(modes)

    for(int idx=0; idx<castToMapper(m_helper)->axes(); idx++)
    {
        m_vectorSCM_tmp[castToMapper(m_helper)->toHw(idx)] = modes[idx];
    }

    ReturnValue ret = m_iraw->setControlModesRaw(m_vectorSCM_tmp);

    return ret;
}
