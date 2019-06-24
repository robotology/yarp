/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Random.h>

#include <cmath>
#include <cstdlib>
#include <random>

using namespace yarp::os;
std::default_random_engine randengine;

double Random::uniform()
{
    std::uniform_real_distribution<double> udist(0.0, 1.0);
    return udist(randengine);
}

double Random::normal()
{
    return normal(0.0, 1.0);
}

double Random::normal(double m, double s)
{
    std::normal_distribution<double> ndist(m, s);
    return ndist(randengine);
}

void Random::seed(int seed)
{
    randengine.seed(seed);
}


int Random::uniform(int min, int max)
{
    std::uniform_int_distribution<int> udist(min, max);
    return udist(randengine);
}
