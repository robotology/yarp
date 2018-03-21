/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Semaphore.h>
#include <yarp/math/Rand.h>
#include <ctime>
#include <cstdio>
#include <cmath>

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

Matrix Rand::matrix(int rows, int cols)
{
    yarp::sig::Matrix ret(rows,cols);
    for(int r=0;r<rows;r++)
        for(int c=0;c<cols;c++)
        {
            ret[r][c]=theRandScalar.get();
        }

    return ret;
}
