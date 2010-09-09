// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Lorenzo Natale
* Copyright (C) 2007, 2010 The RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

// Sept. 2010 Uses gsl routines for random generation.

#include <yarp/os/Semaphore.h>
#include <yarp/math/Rand.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

#include <gsl/gsl_rng.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;
using namespace yarp::math::impl;

/*
* This class was used in the past to wrap random generation
* routines that were not thread safe. Nowdays it could be no 
* longer required because gsl routines are already declared to be 
* thread safe.
*/
class ThreadSafeRandScalar : public RandScalar
{
    yarp::os::Semaphore mutex;
public:
    ThreadSafeRandScalar(): RandScalar()
    {

    }

    void init()        
    {
        mutex.wait();
        RandScalar::init();
        mutex.post();
    }

    void init(int s)
    {
        mutex.wait();
        RandScalar::init(s);
        mutex.post();
    }

    double get(double min=0.0, double max=1.0)
    {
        double ret;
        mutex.wait();
        ret=RandScalar::get(min, max);
        mutex.post();
        return ret;
    }

} theRandScalar;

double Rand::scalar()
{
    return theRandScalar.get();
}

double Rand::scalar(double min, double max)
{
    return theRandScalar.get(min, max);
}

void Rand::init()
{
    theRandScalar.init();
}

void Rand::init(int seed)
{
    theRandScalar.init(seed);
}

Vector Rand::vector(int s)
{
    yarp::sig::Vector ret((size_t) s);
    for(int k=0;k<s;k++)
    {
        ret[k]=theRandScalar.get();
    }

    return ret;
}

Vector Rand::vector(const Vector &min, const Vector &max)
{
    int s=min.size();
    yarp::sig::Vector ret(s);
    for(int k=0;k<s;k++)
    {
        ret[k]=theRandScalar.get(min[k], max[k]);
    }

    return ret;
}

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

RandnScalar::RandnScalar()
{
    y[0] = 0.0;
    y[1] = 0.0;
    last = 2;
}

void RandnScalar::init()
{
    rnd.init();
    last=2;
}

void RandnScalar::init(int aSeed)
{
    rnd.init(aSeed);
    last = 2;
}

double RandnScalar::get()
{
    double ret;
    if (last > 1)
    {
        boxMuller();    //compute two normally distr random number
        last = 0;
    }

    ret = y[last];
    last++;
    return ret;
}

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


// initialize with a call to "time"
void RandScalar::init()
{
    // initialize with time
    int t=(int)time(0);
    RandScalar::init(t);
}

void RandScalar::init(int s)
{
    gsl_rng_set(implementation(impl), s);
}

using namespace yarp::sig;

RandVector::RandVector(int s)
{
    data.resize(s);
}

void RandVector::resize(int s)
{
    data.resize(s);
}

void RandVector::init()
{
    rnd.init();
}

void RandVector::init(int seed)
{
    rnd.init(seed);
}

const Vector &RandVector::get()
{
    for (int k=0;k<data.size(); k++)
    {
        data[k]=rnd.get();
    }

    return data;
}

const Vector &RandVector::get(const Vector &min, const Vector &max)
{
    for (int k=0;k<data.size(); k++)
    {
        data[k]=rnd.get(min[k], max[k]);
    }

    return data;
}

RandnVector::RandnVector(int s)
{
    data.resize(s);
}

void RandnVector::resize(int s)
{
    data.resize(s);
}

void RandnVector::init()
{
    rnd.init();
}

void RandnVector::init(int seed)
{
    rnd.init(seed);
}

const Vector &RandnVector::get()
{
    for (int k=0;k<data.size(); k++)
    {
        data[k]=rnd.get();
    }

    return data;
}

