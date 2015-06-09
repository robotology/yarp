// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <stdio.h>
using namespace yarp::dev;

////////////////////////
// Encoder Interface Timed Implementation
ImplementRemoteVariables::ImplementRemoteVariables(IRemoteVariablesRaw *y)
{
    ivar=y;
    helper = 0;
    temp1=0;
    temp2=0;
}

ImplementRemoteVariables::~ImplementRemoteVariables()
{
    uninitialize();
}

bool ImplementRemoteVariables::initialize(int size, const int *amap)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, 0, 0, 0));
    yAssert (helper != 0);
    temp1=new double [size];
    yAssert (temp1 != 0);
    temp2=new double [size];
    yAssert (temp2 != 0);
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementRemoteVariables::uninitialize()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
    
    checkAndDestroy(temp1);
    checkAndDestroy(temp2);

    return true;
}

bool ImplementRemoteVariables::getRemoteVariable(yarp::os::ConstString key, yarp::os::Bottle& val)
{
    bool ret;
    ret = ivar->getRemoteVariableRaw(key, val);
    return ret;
}

bool ImplementRemoteVariables::setRemoteVariable(yarp::os::ConstString key, const yarp::os::Bottle& val)
{
    bool ret;
    ret = ivar->setRemoteVariableRaw(key, val);
    return ret;
}


bool ImplementRemoteVariables::getRemoteVariablesList(yarp::os::Bottle* listOfKeys)
{
    bool ret;
    ret = ivar->getRemoteVariablesListRaw(listOfKeys);
    return ret;
}
