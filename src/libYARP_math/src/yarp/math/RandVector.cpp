/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/math/RandVector.h>
#include <yarp/sig/Vector.h>

using namespace yarp::sig;
using namespace yarp::math;
using namespace yarp::math::impl;

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
    for (size_t k=0;k<data.size(); k++)
    {
        data[k]=rnd.get();
    }

    return data;
}

const Vector &RandVector::get(const Vector &min, const Vector &max)
{
    for (size_t k=0;k<data.size(); k++)
    {
        data[k]=rnd.get(min[k], max[k]);
    }

    return data;
}
