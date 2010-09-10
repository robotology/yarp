// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Lorenzo Natale
 * Copyright (C) 2007, 2010 The RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARP_MATH_RANDNSCALAR__
#define __YARP_MATH_RANDNSCALAR__

#include <yarp/math/RandScalar.h>

namespace yarp {
    namespace math { 
        namespace impl {
            class RandnScalar;
        }
    }
}

/**
* A random number generator, normal distribution.
* Uses the RandScalar class and the Box-Muller algorithm.
*/
class yarp::math::impl::RandnScalar
{
    void *impl;
    long seed;

private:
    RandnScalar(const RandnScalar &l);
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
