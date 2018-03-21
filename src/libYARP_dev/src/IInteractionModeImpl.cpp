/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/IInteractionModeImpl.h>

using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementInteractionMode::ImplementInteractionMode(yarp::dev::IInteractionModeRaw *class_p) :
    iInteraction(class_p),
    helper(nullptr),
    temp_int(nullptr),
    temp_modes(nullptr)
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

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,nullptr));
    yAssert(helper != nullptr);

    temp_int=new int [size];
    yAssert(temp_int != nullptr);

    temp_modes=new yarp::dev::InteractionModeEnum [size];
    yAssert(temp_modes != nullptr);
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

    checkAndDestroy(temp_int);
    checkAndDestroy(temp_modes);
    return true;
}

bool ImplementInteractionMode::getInteractionMode(int axis, yarp::dev::InteractionModeEnum* mode)
{
    int j = castToMapper(helper)->toHw(axis);
    return iInteraction->getInteractionModeRaw(j, mode);
}

bool ImplementInteractionMode::getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    for (int i = 0; i < n_joints; i++)
    {
         MJOINTIDCHECK(i)
         temp_int[i] = castToMapper(helper)->toHw(joints[i]);
    }
    return iInteraction->getInteractionModesRaw(n_joints, temp_int, modes);
}

bool ImplementInteractionMode::getInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    if(!iInteraction->getInteractionModesRaw(temp_modes) )
        return false;

    for(int idx=0; idx<castToMapper(helper)->axes(); idx++)
    {
        int j = castToMapper(helper)->toUser(idx);
        modes[j] = temp_modes[idx];
    }
    return true;
}

bool ImplementInteractionMode::setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode)
{
    int j = castToMapper(helper)->toHw(axis);
    return iInteraction->setInteractionModeRaw(j, mode);
}

bool ImplementInteractionMode::setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    for(int idx=0; idx<n_joints; idx++)
    {
        MJOINTIDCHECK(idx)
        temp_int[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    return iInteraction->setInteractionModesRaw(n_joints, temp_int, modes);
}

bool ImplementInteractionMode::setInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    for(int idx=0; idx< castToMapper(helper)->axes(); idx++)
    {
        int j = castToMapper(helper)->toHw(idx);
        modes[j] = temp_modes[idx];
    }
    return iInteraction->setInteractionModesRaw(temp_modes);
}
