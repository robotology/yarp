// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <stdio.h>
using namespace yarp::dev;

ImplementControlMode2::ImplementControlMode2(IControlMode2Raw *r):
temp_int(NULL),
temp_mode(NULL)
{
    raw=r;
    helper=0;
}

bool ImplementControlMode2::initialize(int size, const int *amap)
{
    if (helper!=0)
        return false;
    
    double *dummy=new double [size];
    for(int k=0;k<size;k++)
        dummy[k]=0;

    helper=(void *)(new ControlBoardHelper(size, amap, dummy, dummy, dummy));
    _YARP_ASSERT (helper != 0);

    temp_int=new int [size];
    _YARP_ASSERT(temp_int != NULL);

    temp_mode=new int [size];
    _YARP_ASSERT(temp_mode != NULL);

    delete [] dummy;
    return true;
}

ImplementControlMode2::~ImplementControlMode2()
{
    uninitialize();
}

bool ImplementControlMode2::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
 
    checkAndDestroy(temp_int);
    checkAndDestroy(temp_mode);
    return true;
}


bool ImplementControlMode2::setPositionMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setPositionModeRaw(k);
}

bool ImplementControlMode2::setVelocityMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setVelocityModeRaw(k);
}

bool ImplementControlMode2::setTorqueMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setTorqueModeRaw(k);
}

bool ImplementControlMode2::setOpenLoopMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setOpenLoopModeRaw(k);
}

bool ImplementControlMode2::setImpedancePositionMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setImpedancePositionModeRaw(k);
}

bool ImplementControlMode2::setImpedanceVelocityMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setImpedanceVelocityModeRaw(k);
}

bool ImplementControlMode2::getControlMode(int j, int *f)
{
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
        temp_int[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = raw->getControlModesRaw(n_joint, temp_int, temp_mode);

    return ret;
}

bool ImplementControlMode2::setControlMode(const int j, const int mode)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setControlModeRaw(k, mode);
}

bool ImplementControlMode2::setControlModes(const int n_joint, const int *joints, int *modes)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        temp_int[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    return raw->setControlModesRaw(n_joint, temp_int, modes);
}

bool ImplementControlMode2::setControlModes(int *modes)
{
    return raw->setControlModesRaw(modes);
}

