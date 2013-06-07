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


//ImplementControlLimits2::ImplementControlLimits2(IControlLimit2Raw *y)
//{
//    iLimit2 = y;
//    helper = 0;
//    temp_double = 0;
//    temp_int = 0;
//}


ImplementControlLimits2::~ImplementControlLimits2()
{
    uninitialize();
}


bool ImplementControlLimits2::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if(helper!=0)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,0));
    _YARP_ASSERT(helper != 0);
    temp_double=new double [size];
    _YARP_ASSERT(temp_double != 0);

    temp_int=new int [size];
    _YARP_ASSERT(temp_int != 0);
    return true;
}


bool ImplementControlLimits2::setLimits(int axis, double min, double max)
{

}

bool ImplementControlLimits2::setLimits(int n_axis, int *axes, double *min, double *max)
{

}

bool ImplementControlLimits2::setLimits(double *min, double *max)
{

}

bool ImplementControlLimits2::getLimits(int axis, double *min, double *max)
{

}

bool ImplementControlLimits2::getLimits(int n_axis, int *axes, double *min, double *max)
{

}

bool ImplementControlLimits2::getLimits(double *min, double *max)
{

}

bool ImplementControlLimits2::setVelLimits(int axis, double *min, double *max)
{

}

bool ImplementControlLimits2::setVelLimits(int n_axis, int *axes, double *min, double *max)
{

}

bool ImplementControlLimits2::setVelLimits(double *min, double *max)
{

}

bool ImplementControlLimits2::getVelLimits(int axis, double *min, double *max)
{

}

bool ImplementControlLimits2::getVelLimits(int n_axis, int *axes, double *min, double *max)
{

}

bool ImplementControlLimits2::getVelLimits(double *min, double *max)
{

}


