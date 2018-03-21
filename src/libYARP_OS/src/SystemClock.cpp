/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/SystemClock.h>
#include <yarp/conf/system.h>

#include <chrono>
#include <thread>

void yarp::os::SystemClock::delaySystem(double seconds)
{
#if defined _MSC_VER && _MSC_VER <= 1800
    std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(seconds)));
#else
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
#endif
}

double yarp::os::SystemClock::nowSystem()
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}
