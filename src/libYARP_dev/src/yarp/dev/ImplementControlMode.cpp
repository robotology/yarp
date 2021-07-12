/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ImplementControlMode.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cstdio>
using namespace yarp::dev;
using namespace yarp::os;

#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementControlMode::ImplementControlMode(IControlModeRaw *r):
    helper(nullptr),
    raw(r),
    buffManager(nullptr)
{;}

bool ImplementControlMode::initialize(int size, const int *amap)
{
    if (helper!=nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap));
    yAssert (helper != nullptr);

    buffManager = new yarp::dev::impl::FixedSizeBuffersManager<int> (size);
    yAssert (buffManager != nullptr);
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

    if (buffManager!=nullptr)
    {
        delete buffManager;
        buffManager=nullptr;
    }
    return true;
}

bool ImplementControlMode::getControlMode(int j, int *f)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
    return raw->getControlModeRaw(k, f);
}

bool ImplementControlMode::getControlModes(int *modes)
{
    yarp::dev::impl::Buffer<int> buffValues = buffManager->getBuffer();

    bool ret=raw->getControlModesRaw(buffValues.getData());
    castToMapper(helper)->toUser(buffValues.getData(), modes);

    buffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementControlMode::getControlModes(const int n_joint, const int *joints, int *modes)
{
    if(!castToMapper(helper)->checkAxesIds(n_joint, joints))
        return false;

    yarp::dev::impl::Buffer<int> buffValues = buffManager->getBuffer();

    for(int idx=0; idx<n_joint; idx++)
    {
        buffValues[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    bool ret = raw->getControlModesRaw(n_joint, buffValues.getData(), modes);

    buffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementControlMode::setControlMode(const int j, const int mode)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
    return raw->setControlModeRaw(k, mode);
}

bool ImplementControlMode::setControlModes(const int n_joint, const int *joints, int *modes)
{
    if(!castToMapper(helper)->checkAxesIds(n_joint, joints))
        return false;

    yarp::dev::impl::Buffer<int> buffValues  = buffManager->getBuffer();

    for(int idx=0; idx<n_joint; idx++)
    {
        buffValues[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    bool ret = raw->setControlModesRaw(n_joint, buffValues.getData(), modes);

    buffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementControlMode::setControlModes(int *modes)
{
    yarp::dev::impl::Buffer<int> buffValues  = buffManager->getBuffer();
    for(int idx=0; idx<castToMapper(helper)->axes(); idx++)
    {
        buffValues[castToMapper(helper)->toHw(idx)] = modes[idx];
    }
    bool ret = raw->setControlModesRaw(buffValues.getData());
    buffManager->releaseBuffer(buffValues);
    return ret;
}
