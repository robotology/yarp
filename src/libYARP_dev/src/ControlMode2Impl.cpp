/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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

ImplementControlMode2::ImplementControlMode2(IControlMode2Raw *r):
temp_int(nullptr),
temp_mode(nullptr)
{
    raw=r;
    helper=nullptr;
}

bool ImplementControlMode2::initialize(int size, const int *amap)
{
    if (helper!=nullptr)
        return false;

    double *dummy=new double [size];
    for(int k=0;k<size;k++)
        dummy[k]=0;

    helper=(void *)(new ControlBoardHelper(size, amap, dummy, dummy, dummy));
    yAssert (helper != nullptr);

    temp_int=new int [size];
    yAssert(temp_int != nullptr);

    temp_mode=new int [size];
    yAssert(temp_mode != nullptr);

    delete [] dummy;
    return true;
}

ImplementControlMode2::~ImplementControlMode2()
{
    uninitialize();
}

bool ImplementControlMode2::uninitialize ()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

    checkAndDestroy(temp_int);
    checkAndDestroy(temp_mode);
    return true;
}

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
bool ImplementControlMode2::setPositionMode(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    return raw->setPositionModeRaw(k);
YARP_WARNING_POP
}

bool ImplementControlMode2::setVelocityMode(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    return raw->setVelocityModeRaw(k);
YARP_WARNING_POP
}

bool ImplementControlMode2::setTorqueMode(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    return raw->setTorqueModeRaw(k);
YARP_WARNING_POP
}

bool ImplementControlMode2::setImpedancePositionMode(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    return raw->setImpedancePositionModeRaw(k);
YARP_WARNING_POP
}

bool ImplementControlMode2::setImpedanceVelocityMode(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    return raw->setImpedanceVelocityModeRaw(k);
YARP_WARNING_POP
}
#endif // YARP_NO_DEPRECATED

bool ImplementControlMode2::getControlMode(int j, int *f)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
    return raw->getControlModeRaw(k, f);
}

bool ImplementControlMode2::getControlModes(int *modes)
{
    int nj = castToMapper(helper)->axes();
    int *tmp=new int [nj];
    bool ret=raw->getControlModesRaw(tmp);
    castToMapper(helper)->toUser(tmp, modes);
    delete [] tmp;
    return ret;
}

bool ImplementControlMode2::getControlModes(const int n_joint, const int *joints, int *modes)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK
        temp_int[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = raw->getControlModesRaw(n_joint, temp_int, temp_mode);

    return ret;
}

bool ImplementControlMode2::setControlMode(const int j, const int mode)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
    return raw->setControlModeRaw(k, mode);
}

bool ImplementControlMode2::setControlModes(const int n_joint, const int *joints, int *modes)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK
        temp_int[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    return raw->setControlModesRaw(n_joint, temp_int, modes);
}

bool ImplementControlMode2::setControlModes(int *modes)
{
    return raw->setControlModesRaw(modes);
}
