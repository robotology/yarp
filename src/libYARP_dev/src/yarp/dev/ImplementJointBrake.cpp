/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ImplementJointBrake.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cstdio>
using namespace yarp::dev;
using namespace yarp::os;

#define JOINTIDCHECK                         \
    if (j >= castToMapper(helper)->axes()) { \
        yError("joint id out of bound");     \
        return ReturnValue::return_code::return_value_error_method_failed; \
    }

ImplementJointBrake::ImplementJointBrake(IJointBrakeRaw* r) :
    helper(nullptr),
    raw(r)
{}

bool ImplementJointBrake::initialize(int size, const int* amap)
{
    if (helper != nullptr) {
        return false;
    }

    helper=(void *)(new ControlBoardHelper(size, amap));
    yAssert (helper != nullptr);

    return true;
}

ImplementJointBrake::~ImplementJointBrake()
{
    uninitialize();
}

bool ImplementJointBrake::uninitialize()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

    return true;
}

ReturnValue ImplementJointBrake::isJointBraked(int j, bool& braked) const
{
    JOINTIDCHECK
    int k = castToMapper(helper)->toHw(j);
    return raw->isJointBrakedRaw(k, braked);
}

ReturnValue ImplementJointBrake::setManualBrakeActive(int j, bool active)
{
    JOINTIDCHECK
    int k = castToMapper(helper)->toHw(j);
    return raw->setManualBrakeActiveRaw(k, active);
}

ReturnValue ImplementJointBrake::setAutoBrakeEnabled(int j, bool enabled)
{
    JOINTIDCHECK
    int k = castToMapper(helper)->toHw(j);
    return raw->setAutoBrakeEnabledRaw(k, enabled);
}

ReturnValue ImplementJointBrake::getAutoBrakeEnabled(int j, bool& enabled) const
{
    JOINTIDCHECK
    int k = castToMapper(helper)->toHw(j);
    return raw->getAutoBrakeEnabledRaw(k, enabled);
}
