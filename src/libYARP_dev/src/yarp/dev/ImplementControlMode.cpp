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
    helper(nullptr),
    raw(r)
{}

bool ImplementControlMode::initialize(int size, const int *amap)
{
    if (helper != nullptr) {
        return false;
    }

    helper=(void *)(new ControlBoardHelper(size, amap));
    yAssert (helper != nullptr);

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
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

    return true;
}

ReturnValue ImplementControlMode::getAvailableControlModes(int j, std::vector<yarp::dev::SelectableControlModeEnum>& avail)
{
    JOINTIDCHECK(MAPPER_MAXID)
    std::lock_guard lock(m_imp_mutex);

    int k=castToMapper(helper)->toHw(j);
    return raw->getAvailableControlModesRaw(k, avail);
}

ReturnValue ImplementControlMode::getControlMode(int j, yarp::dev::ControlModeEnum& mode)
{
    JOINTIDCHECK(MAPPER_MAXID)
    std::lock_guard lock(m_imp_mutex);

    int k=castToMapper(helper)->toHw(j);
    return raw->getControlModeRaw(k, mode);
}

ReturnValue ImplementControlMode::getControlModes(std::vector<yarp::dev::ControlModeEnum>& modes)
{
    std::lock_guard lock(m_imp_mutex);
    ReturnValue ret=raw->getControlModesRaw(m_vectorCM_tmp);

    for(int idx=0; idx<castToMapper(helper)->axes(); idx++)
    {
        modes[idx] = m_vectorCM_tmp[castToMapper(helper)->toHw(idx)];
    }

    return ret;
}

ReturnValue ImplementControlMode::getControlModes(std::vector<int> joints, std::vector<yarp::dev::ControlModeEnum>& modes)
{
    JOINTSIDVECCHECK
    std::lock_guard lock(m_imp_mutex);

    std::vector<int> vectorInt_tmp(joints.size());
    for(int idx=0; idx<joints.size(); idx++)
    {
        vectorInt_tmp[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    ReturnValue ret = raw->getControlModesRaw(vectorInt_tmp, modes);

    return ret;
}

ReturnValue ImplementControlMode::setControlMode(int j, yarp::dev::SelectableControlModeEnum mode)
{
    JOINTIDCHECK(MAPPER_MAXID)
    std::lock_guard lock(m_imp_mutex);

    int k=castToMapper(helper)->toHw(j);
    return raw->setControlModeRaw(k, mode);
}

ReturnValue ImplementControlMode::setControlModes(std::vector<int> joints, std::vector<yarp::dev::SelectableControlModeEnum> modes)
{
    JOINTSIDVECCHECK
    std::lock_guard lock(m_imp_mutex);

    std::vector<int> vectorInt_tmp(joints.size());
    for(int idx=0; idx<joints.size(); idx++)
    {
        vectorInt_tmp[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    ReturnValue ret = raw->setControlModesRaw(vectorInt_tmp, modes);

    return ret;
}

ReturnValue ImplementControlMode::setControlModes(std::vector<yarp::dev::SelectableControlModeEnum> modes)
{
    std::lock_guard lock(m_imp_mutex);

    for(int idx=0; idx<castToMapper(helper)->axes(); idx++)
    {
        m_vectorSCM_tmp[castToMapper(helper)->toHw(idx)] = modes[idx];
    }

    ReturnValue ret = raw->setControlModesRaw(m_vectorSCM_tmp);

    return ret;
}
