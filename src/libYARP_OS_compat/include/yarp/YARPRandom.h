// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
//
// YARPRandomNumber.h
//

#ifndef __YARPRandomNumberh__
#define __YARPRandomNumberh__

/**
 * \file YARPRandom.h It contains methods and classes for generating random numbers with
 * various distributions.
 */

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <stdlib.h>

//
// normal distribution random number generator.
// - it might be slow, the standard rand() is used as source.
// - beware of initializing the seed of rand.

// original code copyright reported below.

/* boxmuller.c           
   Implements the Polar form of the Box-Muller
   Transformation

   (c) Copyright 1994, Everett F. Carter Jr.
   Permission is granted by the author to use
   this software for any application provided this
   copyright notice is preserved.

*/

#include <math.h>

//extern float ranf();         /* ranf() is uniform in 0..1 */

/**
 * YARPRandom is a container class for random number generation algorithms.
 */
class YARPRandom
{
public:
	/**
	 * Generates a random number in the range 0 to 1.
	 * @return a random floating point number in the range 0 to 1.
	 */
	static double ranf ()
	{
		return double (ACE_OS::rand ()) / double (RAND_MAX);
	}

	/**
	 * Same as ranf().
	 * @return a random floating point number in the range 0 to 1.
	 */
	static double Uniform ()
    { return ranf(); }

	/**
	 * A normal random number generator.
	 * @param m is the mean of the gaussian distribution.
	 * @param s is the standard deviation of the distribution.
	 * @return a random number with normal distribution.
	 */
	static double box_muller(double m, double s)	
	{				        
		float x1, x2, w, y1;
		static double y2;
		static int use_last = 0;

		if (use_last)		        /* use value from previous call */
            {
                y1 = y2;
                use_last = 0;
            }
		else
            {
                do {
                    x1 = 2.0 * ranf() - 1.0;
                    x2 = 2.0 * ranf() - 1.0;
                    w = x1 * x1 + x2 * x2;
                } while ( w >= 1.0 );

                w = sqrt( (-2.0 * log( w ) ) / w );
                y1 = x1 * w;
                y2 = x2 * w;
                use_last = 1;
            }

		return( m + y1 * s );
	}

public:
	/**
	 * Sets the seed of the random number generator.
	 * @param seed is the new seed of the random number generator.
	 */
	static inline void Seed (int seed) { ACE_OS::srand (seed); }

	/**
	 * Sets the seed of the random number generator.
	 * @return a random number normally distributed with 0 mean and 
	 * unitary standard deviation.
	 */
	static inline double RandN () { return box_muller (0.0, 1.0); }

	/**
	 * Sets the seed of the random number generator.
	 * @return a random number uniformly distributed in the range 0 to 1.
	 */
	static inline double RandOne () { return double (ACE_OS::rand()) / double (RAND_MAX); }

	/**
	 * The system rand.
	 * @return a random integer number in the range 0 to RAND_MAX.
	 */
	static inline int Rand () { return ACE_OS::rand (); }

	/**
	 * A random number in a specified range.
	 * @param min is the minimum of the range.
	 * @param max is the maximum of the range.
	 * @return a random integer in the specified range.
	 */
	static inline int Rand (int min, int max);
};

inline int YARPRandom::Rand (int min, int max)
{
	int ret = int ((double (ACE_OS::rand()) / double (RAND_MAX)) * (max - min + 1) + min);

	// there's a small chance the value is = max+1
	if (ret <= max)
		return ret;
	else
		return max;
}

#endif	// __YARPRandomNumberh__
