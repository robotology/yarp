// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <stdio.h>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>

using namespace yarp::dev;

ImplementPositionDirect::ImplementPositionDirect(IPositionDirectRaw *y) :
    iPDirect(y),
    helper(NULL),
    temp_int(NULL),
    temp_double(NULL)
{
}

ImplementPositionDirect::~ImplementPositionDirect()
{
    uninitialize();
}

bool ImplementPositionDirect::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if(helper != NULL)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,0));
    yAssert(helper != NULL);

    temp_double=new double [size];
    yAssert(temp_double != NULL);

    temp_int=new int [size];
    yAssert(temp_int != NULL);
    return true;
}

bool ImplementPositionDirect::uninitialize()
{
    if(helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }

    checkAndDestroy(temp_double);
    checkAndDestroy(temp_int);

    return true;
}

bool ImplementPositionDirect::getAxes(int *axes)
{
    (*axes)=castToMapper(helper)->axes();
    return true;
}

bool ImplementPositionDirect::setPositionDirectMode()
{
    return iPDirect->setPositionDirectModeRaw();
}

bool ImplementPositionDirect::setPosition(int j, double ref)
{
    int k;
    double enc;
    castToMapper(helper)->posA2E(ref, j, enc, k);
    return iPDirect->setPositionRaw(k, enc);
}

bool ImplementPositionDirect::setPositions(const int n_joint, const int *joints, double *refs)
{
    for(int idx=0; idx<n_joint; idx++)
    {
      castToMapper(helper)->posA2E(refs[idx], joints[idx], temp_double[idx], temp_int[idx]);
    }
    return iPDirect->setPositionsRaw(n_joint, temp_int, temp_double);
}

bool ImplementPositionDirect::setPositions(const double *refs)
{
    castToMapper(helper)->posA2E(refs, temp_double);

    return iPDirect->setPositionsRaw(temp_double);
}


// Stub impl

bool StubImplPositionDirectRaw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func)
        fprintf(stderr, "%s:", func);
    else
        fprintf(stderr, "Function ");

    fprintf(stderr, "not yet implemented\n");

    return false;
}
