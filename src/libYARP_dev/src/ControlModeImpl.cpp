/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <cstdio>
using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK if (joints[idx] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementControlMode::ImplementControlMode(IControlModeRaw *r)
{
    raw=r;
    helper=nullptr;
}

bool ImplementControlMode::initialize(int size, const int *amap)
{
    if (helper!=nullptr)
        return false;

    double *dummy=new double [size];
    for(int k=0;k<size;k++)
        dummy[k]=0;

    helper=(void *)(new ControlBoardHelper(size, amap, dummy, dummy, dummy));
    yAssert (helper != nullptr);

    delete [] dummy;
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

    return true;
}

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
bool ImplementControlMode::setPositionMode(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    return raw->setPositionModeRaw(k);
YARP_WARNING_POP
}

bool ImplementControlMode::setVelocityMode(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    return raw->setVelocityModeRaw(k);
YARP_WARNING_POP
}

bool ImplementControlMode::setTorqueMode(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    return raw->setTorqueModeRaw(k);
YARP_WARNING_POP
}

bool ImplementControlMode::setImpedancePositionMode(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    return raw->setImpedancePositionModeRaw(k);
YARP_WARNING_POP
}

bool ImplementControlMode::setImpedanceVelocityMode(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    return raw->setImpedanceVelocityModeRaw(k);
YARP_WARNING_POP
}
#endif // YARP_NO_DEPRECATED

bool ImplementControlMode::getControlMode(int j, int *f)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
    return raw->getControlModeRaw(k, f);
}

bool ImplementControlMode::getControlModes(int *modes)
{
    int nj = castToMapper(helper)->axes();
    int *tmp=new int [nj];
    bool ret=raw->getControlModesRaw(tmp);
    castToMapper(helper)->toUser(tmp, modes);
    delete [] tmp;
    return ret;
}
