// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/SystemClock.h>
#include <yarp/os/impl/PlatformTime.h>

#ifdef ACE_WIN32
// for WIN32 MM functions
#include <mmsystem.h>
#endif

using namespace yarp::os;

/// WARNING: actual precision under WIN32 depends on setting scheduler 
/// by means of MM functions.
///
void SystemClock::delaySystem(double seconds) {
#ifdef YARP_HAS_ACE
    ACE_Time_Value tv;
    tv.sec (long(seconds));
    tv.usec (long((seconds-long(seconds)) * 1.0e6));
    ACE_OS::sleep(tv);
#else
    usleep(seconds*1000000);
#endif
}

double SystemClock::nowSystem() {
#ifdef ACE_WIN32
    // only uses high res on Microsoft Windows
    // This caused problems; maybe only with new ACE versions or Windows 7, can't tell.
    // Lorenzo
	// ACE_Time_Value timev = ACE_High_Res_Timer::gettimeofday_hr();
    ACE_Time_Value timev=ACE_OS::gettimeofday ();
    //ACE_Time_Value timev = ACE_OS::gettimeofday ();
    return double(timev.sec()) + timev.usec() * 1e-6; 
#else
    // on other operating systems, high res seems dysfunctional
    // which is weird since ACE manual claims it maps naturally 
    // on gettimoday...
#  ifdef YARP_HAS_ACE 
    ACE_Time_Value timev = ACE_OS::gettimeofday ();
    return double(timev.sec()) + timev.usec() * 1e-6; 
#  else
    struct  timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return (double)(currentTime.tv_sec + currentTime.tv_usec/1000000);
#  endif
#endif
}

