/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/Vector.h>
#include <yarp/math/Rand.h>
#include <stdio.h>

using namespace yarp::sig;
using namespace yarp::math;

int main(int argc, const char **)
{
    for(int k=0; k<10; k++)
    {
        double n=Rand::scalar();
        fprintf(stderr, "%lf\n", n);
    }

    for(int k=0; k<10; k++)
    {
        Vector v=Rand::vector(3);
        fprintf(stderr, "%s\n", v.toString().c_str());
    }


    return 0;
}
