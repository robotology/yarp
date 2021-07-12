/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MATH_NORMRAND_H
#define YARP_MATH_NORMRAND_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Rand.h>
#include <yarp/math/api.h>

namespace yarp
{
    namespace math
    {

        /**
        * This class groups routines for random number generation, normal distribution.
        */
        class NormRand;
    }
}


/**
* A static class grouping function for normal random number
* generator. Thread safe.
*
* Methods inside this class provides access to a global instance
* of a RandNScalar object that generates random numbers.
*/
class YARP_math_API yarp::math::NormRand
{
public:
    static double scalar(double u=0.0, double sigma=1.0);
    static yarp::sig::Vector vector(int s, double u=0.0, double sigma=1.0);
    static yarp::sig::Vector vector(const yarp::sig::Vector &u, const yarp::sig::Vector &sigma);

    static yarp::sig::Matrix matrix(int r, int c, double u=0.0, double sigma=1.0);

    /**
    * Initialize the random number generator, with
    * current time (time(0)).
    */
    static void init();

    /**
    * Initialize the random number generator, provide
    * a seed.
    * @param seed: a seed.
    */
    static void init(int seed);
};

#endif // YARP_MATH_NORMRAND_H
