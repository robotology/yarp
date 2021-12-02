/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ImplementJointFault.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cstdio>
using namespace yarp::dev;
using namespace yarp::os;

#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementJointFault::ImplementJointFault(IJointFaultRaw *r):
    helper(nullptr),
    raw(r)
{}

bool ImplementJointFault::initialize(int size, const int *amap)
{
    if (helper != nullptr) {
        return false;
    }

    helper=(void *)(new ControlBoardHelper(size, amap));
    yAssert (helper != nullptr);

    return true;
}

ImplementJointFault::~ImplementJointFault()
{
    uninitialize();
}

bool ImplementJointFault::uninitialize ()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

    return true;
}

bool ImplementJointFault::getLastJointFault(int j, int& fault, std::string& message)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
    return raw->getLastJointFaultRaw(k, fault, message);
}
