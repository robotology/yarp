/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
