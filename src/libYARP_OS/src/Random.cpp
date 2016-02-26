/*
 * Copyright: (C) 1994 Everett F. Carter Jr.
 * CopyPolicy: Preserve copyright notice
 */

#include <yarp/os/Random.h>

#include <math.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <stdlib.h>

using namespace yarp::os;

double Random::uniform() {
    return double (ACE_OS::rand ()) / double (RAND_MAX);
}

double Random::normal() {
    return normal(0.0, 1.0);
}


//
// normal distribution random number generator.
// - it might be slow, the standard rand() is used as source.
// - beware of initializing the seed of rand.

// original code copyright reported below.
// summary --
//   CopyPolicy: Preserve copyright notice
//   Copyright: 1994, Everett F. Carter Jr.

/* boxmuller.c
   Implements the Polar form of the Box-Muller
   Transformation

   (c) Copyright 1994, Everett F. Carter Jr.
   Permission is granted by the author to use
   this software for any application provided this
   copyright notice is preserved.

*/



double Random::normal(double m, double s)
{
    double x1, x2, w, y1;
    static double y2;
    static int use_last = 0;

    if (use_last) { /* use value from previous call */
        y1 = y2;
        use_last = 0;
    } else {
        do {
            x1 = 2.0 * uniform() - 1.0;
            x2 = 2.0 * uniform() - 1.0;
            w = x1 * x1 + x2 * x2;
        } while ( w >= 1.0 );

        w = sqrt( (-2.0 * log( w ) ) / w );
        y1 = x1 * w;
        y2 = x2 * w;
        use_last = 1;
    }

    return( m + y1 * s );
}

void Random::seed(int seed) {
    ACE_OS::srand (seed);
}


int Random::uniform(int min, int max) {
    int ret = int ((double (ACE_OS::rand()) / double (RAND_MAX)) * (max - min + 1) + min);

    // there's a small chance the value is = max+1
    if (ret <= max)
        return ret;
    else
        return max;
}
