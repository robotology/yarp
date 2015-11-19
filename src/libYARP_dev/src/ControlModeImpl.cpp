// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Marco Randazzo and Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <stdio.h>
using namespace yarp::dev;

ImplementControlMode::ImplementControlMode(IControlModeRaw *r)
{
    raw=r;
    helper=0;
}

bool ImplementControlMode::initialize(int size, const int *amap)
{
    if (helper!=0)
        return false;

    double *dummy=new double [size];
    for(int k=0;k<size;k++)
        dummy[k]=0;

    helper=(void *)(new ControlBoardHelper(size, amap, dummy, dummy, dummy));
    yAssert (helper != 0);

    delete [] dummy;
    return true;
}

ImplementControlMode::~ImplementControlMode()
{
    uninitialize();
}

bool ImplementControlMode::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }

    return true;
}

bool ImplementControlMode::setPositionMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setPositionModeRaw(k);
}

bool ImplementControlMode::setVelocityMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setVelocityModeRaw(k);
}

bool ImplementControlMode::setTorqueMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setTorqueModeRaw(k);
}

bool ImplementControlMode::setOpenLoopMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setOpenLoopModeRaw(k);
}

bool ImplementControlMode::setImpedancePositionMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setImpedancePositionModeRaw(k);
}

bool ImplementControlMode::setImpedanceVelocityMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setImpedanceVelocityModeRaw(k);
}

bool ImplementControlMode::getControlMode(int j, int *f)
{
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
