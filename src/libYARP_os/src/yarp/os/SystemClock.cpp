/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/conf/system.h>

#include <yarp/os/SystemClock.h>

#include <chrono>
#include <thread>


double yarp::os::SystemClock::now()
{
    return nowSystem();
}

void yarp::os::SystemClock::delay(double seconds)
{
    delaySystem(seconds);
}

bool yarp::os::SystemClock::isValid() const
{
    return true;
}

void yarp::os::SystemClock::delaySystem(double seconds)
{
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

double yarp::os::SystemClock::nowSystem()
{
    return std::chrono::time_point_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
}
