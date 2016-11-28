/*
* Author: Lorenzo Natale
* Copyright (C) 2007, The RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

// Sept. 2010 Uses gsl routines for random generation.

#include <yarp/math/RandnScalar.h>
#include <yarp/sig/Vector.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

using namespace yarp::sig;
using namespace yarp::math;

inline gsl_rng *implementation(void *t)
{
    return static_cast<gsl_rng *>(t);
}

RandnScalar::RandnScalar()
{
    const gsl_rng_type *T;
    gsl_rng_env_setup();
    T=gsl_rng_default;
    impl=gsl_rng_alloc(T);

    init();
}

RandnScalar::RandnScalar(int seed)
{
    const gsl_rng_type *T;
    gsl_rng_env_setup();
    T=gsl_rng_default;
    impl=gsl_rng_alloc(T);

    init(seed);
}

RandnScalar::~RandnScalar()
{
    gsl_rng_free(implementation(impl));
}

// initialize with a call to "time"
void RandnScalar::init()
{
    // initialize with time
    int t=(int)time(0);
    RandnScalar::init(t);
}

void RandnScalar::init(int s)
{
    seed=s;
    gsl_rng_set(implementation(impl), s);
}

double RandnScalar::get(double u, double sigma)
{
     return gsl_ran_gaussian(implementation(impl), sigma)+u; 
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


