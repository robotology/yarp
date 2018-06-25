/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/ImplementControlMode.h>
#include <yarp/dev/ControlBoardHelper.h>

#include <cstdio>
using namespace yarp::dev;
using namespace yarp::os;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK if (joints[idx] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK_DEL if (joints[idx] >= castToMapper(helper)->axes()){yError("joint id out of bound"); buffManager->releaseBuffer(b); return false;}

ImplementControlMode::ImplementControlMode(IControlModeRaw *r): nj(0)
{
    raw=r;
    helper=nullptr;
}

bool ImplementControlMode::initialize(int size, const int *amap)
{
    if (helper!=nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap));
    yAssert (helper != nullptr);

    buffManager = new yarp::os::FixedSizeBuffersManager<int> (size);
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
    try
    {
        Buffer<int> b;
        int *dataPtr = buffManager->getBuffer(b);

        bool ret=raw->getControlModesRaw(dataPtr);
        castToMapper(helper)->toUser(dataPtr, modes);

        buffManager->releaseBuffer(b);
        return ret;
    }
    catch (const std::bad_alloc& e)
    {
        yFatal() << "ImplementControlMode::getControlModes(int *modes) gets a" << e.what() <<" exception!!";
        return false;
    }
}

bool ImplementControlMode::getControlModes(const int n_joint, const int *joints, int *modes)
{
    try
    {
        Buffer<int> b;
        int *tmp_joints = buffManager->getBuffer(b);

        for(int idx=0; idx<n_joint; idx++)
        {
            MJOINTIDCHECK_DEL
            tmp_joints[idx] = castToMapper(helper)->toHw(joints[idx]);
        }

        bool ret = raw->getControlModesRaw(n_joint, tmp_joints, modes);
        buffManager->releaseBuffer(b);
        return ret;
    }
    catch (const std::bad_alloc& e)
    {
        yFatal() << "ImplementControlMode::getControlModes(const int n_joint, const int *joints, int *modes) gets a" << e.what() <<" exception!!";
        return false;
    }

}

bool ImplementControlMode::setControlMode(const int j, const int mode)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
    return raw->setControlModeRaw(k, mode);
}

bool ImplementControlMode::setControlModes(const int n_joint, const int *joints, int *modes)
{
    try
    {
        Buffer<int> b;
        int *tmp_joints = buffManager->getBuffer(b);

        for(int idx=0; idx<n_joint; idx++)
        {
            MJOINTIDCHECK_DEL
            tmp_joints[idx] = castToMapper(helper)->toHw(joints[idx]);
        }
        bool ret = raw->setControlModesRaw(n_joint, tmp_joints, modes);
        buffManager->releaseBuffer(b);
        return ret;
    }
    catch (const std::bad_alloc& e)
    {
        return false;
    }
}

bool ImplementControlMode::setControlModes(int *modes)
{
    int *modes_tmp=new int [nj];
    for(int idx=0; idx<nj; idx++)
    {
        modes_tmp[castToMapper(helper)->toHw(idx)] = modes[idx];
    }
    bool ret = raw->setControlModesRaw(modes_tmp);
    delete [] modes_tmp;
    return ret;
}
