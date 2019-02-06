/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ImplementInteractionMode.h>

using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK_DEL(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); delete [] temp; return false;}

ImplementInteractionMode::ImplementInteractionMode(yarp::dev::IInteractionModeRaw *class_p) :
    iInteraction(class_p),
    helper(nullptr),
    nj(0)
{

}


ImplementInteractionMode::~ImplementInteractionMode()
{
    uninitialize();
}

/**
 * Allocate memory for internal data
 * @param size the number of joints
 * @param amap axis map for this device wrapper
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementInteractionMode::initialize(int size, const int *amap)
{
    return initialize(size, amap, nullptr, nullptr);
}

bool ImplementInteractionMode::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if(helper != nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert(helper != nullptr);

    nj = size;
    return true;
}

/**
 * Clean up internal data and memory.
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementInteractionMode::uninitialize()
{
    if(helper != nullptr)
    {
        delete castToMapper(helper);
        helper = nullptr;
    }
    return true;
}

bool ImplementInteractionMode::getInteractionMode(int axis, yarp::dev::InteractionModeEnum* mode)
{
    int j = castToMapper(helper)->toHw(axis);
    return iInteraction->getInteractionModeRaw(j, mode);
}

bool ImplementInteractionMode::getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    int *temp =  new int [nj];
    for (int i = 0; i < n_joints; i++)
    {
         MJOINTIDCHECK_DEL(i)
         temp[i] = castToMapper(helper)->toHw(joints[i]);
    }
    bool ret = iInteraction->getInteractionModesRaw(n_joints, temp, modes);
    delete [] temp;
    return ret;
}

bool ImplementInteractionMode::getInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    auto* temp_modes=new yarp::dev::InteractionModeEnum [nj];
    if(!iInteraction->getInteractionModesRaw(temp_modes) )
    {
        delete [] temp_modes;
        return false;
    }
    for(int idx=0; idx<castToMapper(helper)->axes(); idx++)
    {
        int j = castToMapper(helper)->toUser(idx);
        modes[j] = temp_modes[idx];
    }
    delete [] temp_modes;
    return true;
}

bool ImplementInteractionMode::setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode)
{
    int j = castToMapper(helper)->toHw(axis);
    return iInteraction->setInteractionModeRaw(j, mode);
}

bool ImplementInteractionMode::setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    int *temp =  new int [nj];
    for(int idx=0; idx<n_joints; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        temp[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    bool ret = iInteraction->setInteractionModesRaw(n_joints, temp, modes);
    delete [] temp;
    return ret;
}

bool ImplementInteractionMode::setInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    auto* temp_modes=new yarp::dev::InteractionModeEnum [nj];
    for(int idx=0; idx< castToMapper(helper)->axes(); idx++)
    {
        int j = castToMapper(helper)->toHw(idx);
        temp_modes[j] = modes[idx];
    }
    bool ret = iInteraction->setInteractionModesRaw(temp_modes);
    delete [] temp_modes;
    return ret;
}
