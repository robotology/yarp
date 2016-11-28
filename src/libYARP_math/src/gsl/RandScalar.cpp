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

#include <gsl/gsl_rng.h>

using namespace yarp::sig;
using namespace yarp::math;

inline gsl_rng *implementation(void *t)
{
    return static_cast<gsl_rng *>(t);
}

RandScalar::RandScalar()
{
    const gsl_rng_type *T;
    gsl_rng_env_setup();
    T=gsl_rng_default;
    impl=gsl_rng_alloc(T);

    init();
}

RandScalar::RandScalar(int seed)
{
    const gsl_rng_type *T;
    gsl_rng_env_setup();
    T=gsl_rng_default;
    impl=gsl_rng_alloc(T);

    init(seed);
}

RandScalar::~RandScalar()
{
    gsl_rng_free(implementation(impl));
}

double RandScalar::get()
{
    return gsl_rng_uniform(implementation(impl));
}

double RandScalar::get(double min, double max)
{
    double ret=RandScalar::get();
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
    gsl_rng_set(implementation(impl), s);
}
