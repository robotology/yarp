/*
* Author: Lorenzo Natale
* Copyright (C) 2007, The RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

// Sept. 2010 Uses gsl routines for random generation.

#include <yarp/math/RandScalar.h>
#include <yarp/sig/Vector.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

// implementation of Marsenne Twister
#include <yarp/impl/mt.h>

using namespace yarp::sig;
using namespace yarp::math;

inline MersenneTwister *implementation(void *t)
{
    return static_cast<MersenneTwister  *>(t);
}

RandScalar::RandScalar()
{
    impl = new MersenneTwister;

    init();
}

RandScalar::RandScalar(int seed)
{
    impl = new MersenneTwister;
    implementation(impl)->init_genrand(seed);
}

RandScalar::~RandScalar()
{
    delete implementation(impl);
}

double RandScalar::get()
{
    return implementation(impl)->random();
}

double RandScalar::get(double min, double max)
{
    double ret=implementation(impl)->random();
    ret=ret*(max-min)+min;
    return ret;
}

// initialize with a call to "time"
void RandScalar::init()
{
    // initialize with time
    int t=(int)time(0);
    RandScalar::init(t);
}

void RandScalar::init(int s)
{
    seed=s;
    implementation(impl)->init_genrand(seed);
}
