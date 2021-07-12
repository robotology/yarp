/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
}

ImplementAxisInfo::~ImplementAxisInfo()
{
    uninitialize();
}

bool ImplementAxisInfo::initialize(int size, const int *amap)
{
    if (helper!=nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap));
    yAssert (helper != nullptr);

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



    return true;
}

bool ImplementAxisInfo::getAxisName(int axis, std::string& name)
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
