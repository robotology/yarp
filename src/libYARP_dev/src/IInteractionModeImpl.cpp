// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <stdio.h>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/IInteractionModeImpl.h>

using namespace yarp::dev;


ImplementInteractionMode::ImplementInteractionMode(yarp::dev::IInteractionModeRaw *class_p) :
    iInteraction(class_p),
    helper(NULL),
    temp_int(NULL),
    temp_modes(NULL)
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
    return initialize(size, amap, 0, 0);
}

bool ImplementInteractionMode::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if(helper != NULL)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,0));
    _YARP_ASSERT(helper != NULL);

    temp_int=new int [size];
    _YARP_ASSERT(temp_int != NULL);

    temp_modes=new yarp::dev::InteractionModeEnum [size];
    _YARP_ASSERT(temp_modes != NULL);
    return true;
}

/**
 * Clean up internal data and memory.
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementInteractionMode::uninitialize()
{
    if(helper != NULL)
    {
        delete castToMapper(helper);
        helper = NULL;
    }

    checkAndDestroy(temp_int);
    checkAndDestroy(temp_modes);
    return true;
}

bool ImplementInteractionMode::getInteractionMode(int axis, yarp::dev::InteractionModeEnum* mode)
{
    int j = castToMapper(helper)->toUser(axis);
    return iInteraction->getInteractionModeRaw(j, mode);
}

bool ImplementInteractionMode::getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    for(int i=0; i<n_joints; i++)
        temp_int[i] = castToMapper(helper)->toHw(joints[i]);

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

