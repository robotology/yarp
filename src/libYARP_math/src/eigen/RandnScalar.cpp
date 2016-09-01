/*
* Author: Lorenzo Natale
* Copyright (C) 2007, The RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

// Sept. 2016 Uses C++11 routines for random generation.

#include <yarp/math/RandnScalar.h>
#include <yarp/sig/Vector.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

using namespace yarp::sig;
using namespace yarp::math;

RandnScalar::RandnScalar()
{
}

RandnScalar::RandnScalar(int seed)
{
}

RandnScalar::~RandnScalar()
{
}

// initialize with a call to "time"
void RandnScalar::init()
{
}

void RandnScalar::init(int s)
{
}

double RandnScalar::get(double u, double sigma)
{
     return 0.0;
}

#if 0
inline void RandnScalar::boxMuller()
{
    double x1, x2;
    double w = 2.0;
    while (w >= 1.0)
    {
        x1 = 2.0 * rnd.get() - 1.0;
        x2 = 2.0 * rnd.get() - 1.0;
        w = x1 * x1 + x2 * x2;
    }

    w = sqrt( (-2.0 * log( w ) ) / w );
    y[0] = x1 * w;
    y[1] = x2 * w;
}
#endif 


