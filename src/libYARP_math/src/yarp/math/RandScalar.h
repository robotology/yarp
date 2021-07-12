/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MATH_RANDSCALAR_H
#define YARP_MATH_RANDSCALAR_H

#include <yarp/math/api.h>

namespace yarp {
    namespace math {
            class RandScalar;
    }
}

/**
* A random number generator, uniform in the range 0-1.
* This class is a simple wrapper around C++11 Mersenne Twister engine.
*
* The generator is always initialized with a seed equal to the
* current time. You can reset the seed by explicitly calling Rand::init(seed).
*
* Default generator is std::mt19937.
*
*/
class YARP_math_API yarp::math::RandScalar
{
    void *impl;
    int seed;
    RandScalar(const RandScalar &l);
public:
    RandScalar(int seed);
    RandScalar();
    ~RandScalar();

    /**
    * Initialize the random generator using
    * current time (time(0)).
    */
    void init();

    /**
    * Initialize the random generator.
    * @param seed the seed.
    */
    void init (int seed);

    /**
    * Get the seed.
    * @return the seed.
    */
    int getSeed ()
    { return seed; }

    /**
    * Generate a random number from a
    * uniform distribution.
    * @return the random value.
    */
    double get();

    /**
    * Generate a random number from a
    * uniform distribution within [min, max]
    * @return the random value.
    */
    double get(double min, double max);
};

#endif // YARP_MATH_RANDSCALAR_H
