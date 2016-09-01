/*
* Author: Lorenzo Natale
* Copyright (C) 2007, The RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

// Sept. 2016 Uses C++11 routines for random generation.

#include <yarp/math/RandScalar.h>
#include <yarp/sig/Vector.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

using namespace yarp::sig;
using namespace yarp::math;


RandScalar::RandScalar()
{
}

RandScalar::RandScalar(int seed)
{
}

RandScalar::~RandScalar()
{
}

double RandScalar::get()
{
    return 1.0;
}

double RandScalar::get(double min, double max)
{
    return 1.0;
}

// initialize with a call to "time"
void RandScalar::init()
{
}

void RandScalar::init(int s)
{
}
