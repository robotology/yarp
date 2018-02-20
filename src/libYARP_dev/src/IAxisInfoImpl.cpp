/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "yarp/dev/ImplementAxisInfo.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <cstdio>
using namespace yarp::dev;

////////////////////////
// Encoder Interface Timed Implementation
ImplementAxisInfo::ImplementAxisInfo(yarp::dev::IAxisInfoRaw *y)
{
    iinfo=y;
    helper = nullptr;
    temp1=nullptr;
    temp2=nullptr;
}

ImplementAxisInfo::~ImplementAxisInfo()
{
    uninitialize();
}

bool ImplementAxisInfo::initialize(int size, const int *amap)
{
    if (helper!=nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, nullptr, 0, 0));
    yAssert (helper != nullptr);
    temp1=new double [size];
    yAssert (temp1 != nullptr);
    temp2=new double [size];
    yAssert (temp2 != nullptr);
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementAxisInfo::uninitialize()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

    checkAndDestroy(temp1);
    checkAndDestroy(temp2);

    return true;
}

bool ImplementAxisInfo::getAxisName(int axis, yarp::os::ConstString& name)
{
    bool ret;
    int k = castToMapper(helper)->toHw(axis);
    ret = iinfo->getAxisNameRaw(k, name);
    return ret;
}

bool ImplementAxisInfo::getJointType(int axis, yarp::dev::JointTypeEnum& type)
{
    bool ret;
    int k = castToMapper(helper)->toHw(axis);
    ret = iinfo->getJointTypeRaw(k, type);
    return ret;
}

bool ret;
