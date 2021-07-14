/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/math/Rand.h>

#include <ctime>
#include <cstdio>
#include <cmath>
#include <mutex>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;
using namespace yarp::math::impl;

/*
* This class was used in the past to wrap random generation
* routines that were not thread safe. Nowadays it could be no
* longer required because gsl routines are already declared to be
* thread safe.
*/
class ThreadSafeRandScalar : public RandScalar
{
    std::mutex mutex;
public:
    ThreadSafeRandScalar(): RandScalar()
    {

    }

    void init()
    {
        mutex.lock();
        RandScalar::init();
        mutex.unlock();
    }

    void init(int s)
    {
        mutex.lock();
        RandScalar::init(s);
        mutex.unlock();
    }

    double get(double min=0.0, double max=1.0)
    {
        double ret;
        mutex.lock();
        ret=RandScalar::get(min, max);
        mutex.unlock();
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
    size_t s = min.size();
    yarp::sig::Vector ret(s);
    for(size_t k=0;k<s;k++)
    {
        ret[k]=theRandScalar.get(min[k], max[k]);
    }

    return ret;
}

Matrix Rand::matrix(int rows, int cols)
{
    yarp::sig::Matrix ret(rows,cols);
    for (int r = 0; r < rows; r++) {
        for(int c=0;c<cols;c++)
        {
            ret[r][c]=theRandScalar.get();
        }
    }

    return ret;
}
