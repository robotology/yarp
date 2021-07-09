/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/math/RandnScalar.h>
#include <yarp/math/RandScalar.h>
#include <yarp/sig/Vector.h>
#include <ctime>
#include <cstdio>
#include <cmath>

using namespace yarp::sig;
using namespace yarp::math;

inline RandScalar *implementation(void *t)
{
    return static_cast<RandScalar*>(t);
}

RandnScalar::RandnScalar()
{
    impl = new RandScalar;
    init();
}

RandnScalar::RandnScalar(int seed)
{
    impl = new RandScalar;
    init(seed);
}

RandnScalar::~RandnScalar()
{
    delete (implementation(impl));
}

// initialize with a call to "time"
void RandnScalar::init()
{
    // initialize with time
    int t=(int)time(nullptr);
    RandnScalar::init(t);
}

void RandnScalar::init(int s)
{
    //force re-execution of BoxMuller
    executeBoxMuller = true;
    seed=s;
    implementation(impl)->init(s);
}

double RandnScalar::get(double u, double sigma)
{
    // BoxMuller generates two numbers every iteration
    // we return y[0] the first time, and y[1] the second time
    if (executeBoxMuller)
    {
        boxMuller();
        return y[0] * sigma + u;
    }
    else
    {
        executeBoxMuller = true;
        return y[1] * sigma + u;
    }
}

void RandnScalar::boxMuller()
{
    double x1 = 0.0;
    double x2 = 0.0;
    double w = 2.0;

    while (w >= 1.0)
    {
        x1 = 2.0 * implementation(impl)->get() - 1.0;
        x2 = 2.0 * implementation(impl)->get() - 1.0;
        w = x1 * x1 + x2 * x2;
    }

    w = sqrt( (-2.0 * log( w ) ) / w );
    y[0] = x1 * w;
    y[1] = x2 * w;
}
