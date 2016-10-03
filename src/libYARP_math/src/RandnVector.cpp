/*
* Author: Lorenzo Natale
* Copyright (C) 2007, 2010 The RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

// Sept. 2010 Uses gsl routines for random generation.

#include <yarp/math/RandnVector.h>
#include <yarp/sig/Vector.h>

using namespace yarp::sig; 
using namespace yarp::math;
using namespace yarp::math::impl;

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

const Vector &RandnVector::get(double u, double sigma)
{
    for (size_t k=0;k<data.size(); k++)
    {
        data[k]=rnd.get(u, sigma);
    }

    return data;
}

const Vector &RandnVector::get(const Vector &u, const Vector &sigma)
{
    for (size_t k=0;k<data.size(); k++)
    {
        data[k]=rnd.get(u[k], sigma[k]);
    }

    return data;
}


inline void RandnScalar::boxMuller()
{
 //empty this is just for temporary compatibility with the eigen implementation 
}
