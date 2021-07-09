/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
