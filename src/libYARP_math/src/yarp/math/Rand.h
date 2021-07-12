/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MATH_RAND_H
#define YARP_MATH_RAND_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

#include <yarp/math/RandScalar.h>
#include <yarp/math/RandVector.h>
#include <yarp/math/RandnScalar.h>
#include <yarp/math/RandnVector.h>
#include <yarp/math/api.h>

namespace yarp
{
    namespace math
    {
        /**
        * This class groups routines for random number generation, uniform distribution.
        */
       class Rand;
    }
};


/**
* A static class grouping function for uniform random number
* generator. Thread safe.
*
* Methods inside this class provides access to a global instance
* of a RandScalar object that generates random numbers.
*/
class YARP_math_API yarp::math::Rand
{
public:
    /**
    * Get a random number from a uniform distribution
    * in the range [0,1].
    */
    static double scalar();


    /**
    * Get a random number from a uniform distribution
    * in the range [min,max].
    * @param min lower bound
    * @param max upper bound
    * @return return value
    */
    static double scalar(double min, double max);

    /**
    * Get a vector of random numbers from a uniform distribution,
    * values are in the range [0, 1].
    * @param s the size of the vector
    * @return the random vector
    */
    static yarp::sig::Vector vector(int s);


    /**
    * Get a vector of random numbers from a uniform distribution,
    * values are in the range [0, 1].
    * @param r number of rows
    * @param c number of columns
    * @return the random matrix
    */
    static yarp::sig::Matrix matrix(int r, int c);

    /**
    * Get a vector of random numbers from a uniform distribution,
    * values are in the range [min, max], for each components. The
    * function determines the size of the vector from the parameters.
    * @param min a vector whose components specify the lower bound of
    * the distribution.
    * @param max a vector whose components specify the upper bound of
    * the distribution
    * @return the random vector
    */
    static yarp::sig::Vector vector(const yarp::sig::Vector &min,
                                    const yarp::sig::Vector &max);

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

#endif // YARP_MATH_RAND_H
