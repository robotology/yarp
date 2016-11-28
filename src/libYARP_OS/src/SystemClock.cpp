/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick <paulfitz@alum.mit.edu>
 *          Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/SystemClock.h>
#include <yarp/conf/system.h>

#if defined YARP_HAS_CXX11
#  include <chrono>
#  include <thread>
#else
#  include <yarp/os/impl/PlatformTime.h>
#endif

void yarp::os::SystemClock::delaySystem(double seconds)
{
#if defined YARP_HAS_CXX11
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
#elif defined YARP_HAS_ACE
    ACE_Time_Value tv;
    tv.sec (long(seconds));
    tv.usec (long((seconds-long(seconds)) * 1.0e6));
    ACE_OS::sleep(tv);
#else
    usleep(seconds*1000000);
#endif
}

double yarp::os::SystemClock::nowSystem()
{
#if defined YARP_HAS_CXX11
    return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
#elif defined YARP_HAS_ACE
    ACE_Time_Value timev = ACE_OS::gettimeofday();
    return double(timev.sec()) + timev.usec() * 1e-6;
#else
    struct timeval currentTime;
    gettimeofday(&currentTime, YARP_NULLPTR);
    return (double)(currentTime.tv_sec + currentTime.tv_usec * 1e-6);
#endif
}
