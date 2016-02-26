/*
 * Author: Lorenzo Natale
 * Copyright (C) 2007, 2010 The RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_MATH_RANDNVECTOR
#define YARP_MATH_RANDNVECTOR

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

#endif

