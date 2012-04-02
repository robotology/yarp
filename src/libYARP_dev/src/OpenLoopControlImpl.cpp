// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Marco Randazzo and Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "yarp/dev/ImplementOpenLoopControl.h"
#include <yarp/dev/ControlBoardHelper.h>

using namespace yarp::dev;

/////////////// implement ImplementOpenLoopControl
ImplementOpenLoopControl::ImplementOpenLoopControl(IOpenLoopControlRaw *r)
{
    raw=r;
    helper=0;
}

bool ImplementOpenLoopControl::initialize(int size, const int *amap)
{
    if (helper!=0)
        return false;
    
    double *dummy=new double [size];
    for(int k=0;k<size;k++)
        dummy[k]=0;

    helper=(void *)(new ControlBoardHelper(size, amap, dummy, dummy, dummy));
    _YARP_ASSERT(helper != 0);

    delete [] dummy;
    return true;
}

ImplementOpenLoopControl::~ImplementOpenLoopControl()
{
    uninitialize();
}

bool ImplementOpenLoopControl::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
 
    return true;
}

bool ImplementOpenLoopControl::setOutput(int j, double v)
{
    int k=castToMapper(helper)->toHw(j);

    return raw->setOutputRaw(k, v);
}

bool ImplementOpenLoopControl::setOutputs(const double *v)
{
    castToMapper(helper)->toHw(v, dummy);

    return raw->setOutputsRaw(dummy);
}

bool ImplementOpenLoopControl::setOpenLoopMode()
{
    return raw->setOpenLoopModeRaw();
}

bool ImplementOpenLoopControl::getOutputs(double *v)
{
    bool ret=raw->getOutputsRaw(dummy);

    castToMapper(helper)->toUser(dummy, v);
    return ret;
}

bool ImplementOpenLoopControl::getOutput(int j, double *v)
{
    int k=castToMapper(helper)->toHw(j);

    return raw->getOutputRaw(k, v);
}

