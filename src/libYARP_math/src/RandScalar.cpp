/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// Sept. 2010 Uses gsl routines for random generation.

#include <yarp/math/RandScalar.h>
#include <yarp/sig/Vector.h>
#include <ctime>
#include <cstdio>
#include <cmath>

// implementation of Marsenne Twister from C++11's library
#include <random>

using namespace yarp::sig;
using namespace yarp::math;

inline std::mt19937 *implementation(void *t)
{
    return static_cast<std::mt19937 *>(t);
}

RandScalar::RandScalar()
{
    impl = new std::mt19937();
    init();
}

RandScalar::RandScalar(int s) :
    seed(s)
{
    impl = new std::mt19937;
    implementation(impl)->seed(seed);
}

RandScalar::~RandScalar()
{
    delete implementation(impl);
}

double RandScalar::get()
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(*(implementation(impl)));
}

double RandScalar::get(double min, double max)
{
    std::uniform_real_distribution<double> dist(min, max);
    return dist(*(implementation(impl)));
}

// initialize with a call to "time"
void RandScalar::init()
{
    // initialize with time
    int t=(int)time(nullptr);
    RandScalar::init(t);
}

void RandScalar::init(int s)
{
    seed=s;
    implementation(impl)->seed(seed);
}
