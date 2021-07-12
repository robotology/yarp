/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MATH_RANDVECTOR_H
#define YARP_MATH_RANDVECTOR_H

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

#endif // YARP_MATH_RANDVECTOR_H
