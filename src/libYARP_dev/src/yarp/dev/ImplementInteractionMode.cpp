/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ImplementInteractionMode.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

using namespace yarp::dev;
using namespace yarp::os;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementInteractionMode::ImplementInteractionMode(yarp::dev::IInteractionModeRaw *class_p) :
    iInteraction(class_p),
    helper(nullptr),
    imodeBuffManager(nullptr),
    intBuffManager(nullptr)
{;}


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
    if (helper != nullptr) {
        return false;
    }

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert(helper != nullptr);

    intBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<int> (size);
    yAssert (intBuffManager != nullptr);

    imodeBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<yarp::dev::InteractionModeEnum> (size, 1);
    yAssert (imodeBuffManager != nullptr);

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

    if(intBuffManager)
    {
        delete intBuffManager;
        intBuffManager=nullptr;
    }

    if(imodeBuffManager)
    {
        delete imodeBuffManager;
        imodeBuffManager=nullptr;
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
    if (!castToMapper(helper)->checkAxesIds(n_joints, joints)) {
        return false;
    }

    yarp::dev::impl::Buffer<int> buffJoints =  intBuffManager->getBuffer();

    for (int i = 0; i < n_joints; i++)
    {
        buffJoints[i] = castToMapper(helper)->toHw(joints[i]);
    }
    bool ret = iInteraction->getInteractionModesRaw(n_joints, buffJoints.getData(), modes);

    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

bool ImplementInteractionMode::getInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    yarp::dev::impl::Buffer<yarp::dev::InteractionModeEnum> buffValues = imodeBuffManager->getBuffer();
    if(!iInteraction->getInteractionModesRaw(buffValues.getData()) )
    {
        imodeBuffManager->releaseBuffer(buffValues);
        return false;
    }
    for(int idx=0; idx<castToMapper(helper)->axes(); idx++)
    {
        int j = castToMapper(helper)->toUser(idx);
        modes[j] = buffValues[idx];
    }
    imodeBuffManager->releaseBuffer(buffValues);
    return true;
}

bool ImplementInteractionMode::setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode)
{
    int j = castToMapper(helper)->toHw(axis);
    return iInteraction->setInteractionModeRaw(j, mode);
}

bool ImplementInteractionMode::setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    if (!castToMapper(helper)->checkAxesIds(n_joints, joints)) {
        return false;
    }

    yarp::dev::impl::Buffer<int> buffJoints =  intBuffManager->getBuffer();

    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    bool ret = iInteraction->setInteractionModesRaw(n_joints, buffJoints.getData(), modes);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

bool ImplementInteractionMode::setInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    yarp::dev::impl::Buffer<yarp::dev::InteractionModeEnum> buffValues = imodeBuffManager->getBuffer();
    for(int idx=0; idx< castToMapper(helper)->axes(); idx++)
    {
        int j = castToMapper(helper)->toHw(idx);
        buffValues[j] = modes[idx];
    }

    bool ret = iInteraction->setInteractionModesRaw(buffValues.getData());
    imodeBuffManager->releaseBuffer(buffValues);
    return ret;
}
