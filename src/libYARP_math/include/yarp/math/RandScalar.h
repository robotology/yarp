/*
 * Author: Lorenzo Natale
 * Copyright (C) 2010 The RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_MATH_RANDSCALAR
#define YARP_MATH_RANDSCALAR

#include <yarp/math/api.h>

namespace yarp {
    namespace math { 
            class RandScalar;
    }
}

/**
* A random number generator, uniform in the range 0-1.
* This class is a simple wrapper around gsl routines. This probably provides
* an excellent source of random numbers, but if you are picky about 
* random number generation we recommend you read the gsl documentation 
* (see below) or implement your own routines.
* 
* The initialization routine of the generator is basically:
*
* gsl_rng_env_setup();
* T=gsl_rng_default;
* r=gsl_rng_alloc(T);
*
* gsl_rng_set(T, seed);
* 
* which means that the behavior of the random generator can be configured
* through the environment variables GSL_RNG_TYPE as described in the GSL 
* documentation. The genrator is alwasy initialized with a seed equal to the
* current time. You can reset the seed by explicitly calling Rand::init(seed).
*
* Default generator is gsl_rng_mt19937.
*
* See http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Generation.html
* for more information.
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

#endif
