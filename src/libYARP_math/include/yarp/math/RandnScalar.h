/*
 * Author: Lorenzo Natale
 * Copyright (C) 2007, 2010 The RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_MATH_RANDNSCALAR
#define YARP_MATH_RANDNSCALAR

#include <yarp/math/RandScalar.h>
#include <yarp/math/api.h>

namespace yarp {
    namespace math { 
            class RandnScalar;
    }
}

/**
* A random number generator, normal distribution.
* Uses the RandScalar class and the Box-Muller algorithm.
*/
class YARP_math_API yarp::math::RandnScalar
{
    void *impl;
    long seed;

    double y[2];            // required by the boxmuller transform
    bool executeBoxMuller;  // boxMuller transform generates two mubers at the time, this flag determines when to re-use previous.

private:
    RandnScalar(const RandnScalar &l);

    /**
    * Box muller transform, uses uniformly sampled random number to generate
    * gaussian distribution.
    */
    inline void boxMuller();
public:
    /**
    * Constructor.
    */
    RandnScalar();
    ~RandnScalar();
    
    /**
    * Constructor, initialize the generator.
    * @param seed, seed for the rnd generator.
    */
    RandnScalar(int seed);

    /**
    * Inizialize the generator.
    * Uses current time for the seed.
    */
    void init();

    /**
    * Inizialize the generator.
    * Provide a seed.
    * @param seed the seed
    */
    void init (int seed);

    long getSeed ()
    { return seed; }

    /**
    * Generate a randomly generated number, drawn from
    * a normal distribution.
    * @param u mean of the distribution, default 0.0
    * @param sigma sigma of the distribution, default 1.0
    * @return the random number.
    */
    double get(double u=0.0, double sigma=1.0);
};

#endif
