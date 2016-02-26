/*
 * Author: Lorenzo Natale
 * Copyright (C) 2007, 2010 The RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_MATH_RANDVECTOR
#define YARP_MATH_RANDVECTOR

#include <yarp/sig/Vector.h>
#include <yarp/math/RandScalar.h>
#include <yarp/math/api.h>

namespace yarp {
    namespace math { 
        namespace impl {
            class RandVector;
        }
    }
}

/**
* A class to generate random vectors, uniform distribution.
* Internally uses the RandScalar class.
*/
class YARP_math_API yarp::math::impl::RandVector
{
    yarp::sig::Vector data;
    RandScalar rnd;

private:
    RandVector(const RandVector &l);

public:
    RandVector(int s);

    void resize(int s);
    void init();
    void init(int seed);

    const yarp::sig::Vector &get();
    const yarp::sig::Vector &get(const yarp::sig::Vector &min, const yarp::sig::Vector &max);
};

#endif


