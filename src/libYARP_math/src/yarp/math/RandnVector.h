/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MATH_RANDNVECTOR_H
#define YARP_MATH_RANDNVECTOR_H

#include <yarp/sig/Vector.h>
#include <yarp/math/RandnScalar.h>
#include <yarp/math/api.h>

namespace yarp {
    namespace math {
        namespace impl {
            class RandnVector;
        }
    }
}

/**
* A class to generate random vectors, normal distribution.
* Internally uses the RandnScalar class.
*/
class YARP_math_API yarp::math::impl::RandnVector
{
    yarp::sig::Vector data;
    RandnScalar rnd;
    RandnVector(const RandnVector &l){};

public:
    RandnVector(int s);

    void resize(int s);
    void init();
    void init(int seed);

    const yarp::sig::Vector &get(const yarp::sig::Vector &u, const yarp::sig::Vector &sigma);
    const yarp::sig::Vector &get(double u=0.0, double sigma=1.0);
};

#endif // YARP_MATH_RANDNSCALAR_H
