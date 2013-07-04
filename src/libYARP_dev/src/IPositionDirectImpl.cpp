// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
* Author:  Alberto Cardellino
* email:   alberto.cardellino@iit.it
* website: www.robotcub.org
* Released under the terms the GNU General Public License, version 2 or any
* later version published by the Free Software Foundation.
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
