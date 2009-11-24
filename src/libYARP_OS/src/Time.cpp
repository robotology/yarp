// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/Time.h>

//#include <ace/OS.h>
#include <ace/OS_NS_unistd.h>

#include <ace/High_Res_Timer.h>

#ifdef ACE_WIN32
// for WIN32 MM functions
#include <mmsystem.h>
#endif

using namespace yarp::os;

/// WARNING: actual precision under WIN32 depends on setting scheduler 
/// by means of MM functions.
///
void Time::delay(double seconds) {
    ACE_Time_Value tv;
    tv.sec (long(seconds));
    tv.usec (long((seconds-long(seconds)) * 1.0e6));
    ACE_OS::sleep(tv);
}

double Time::now() {
#ifdef ACE_WIN32
    // only uses high res on Microsoft Windows
	ACE_Time_Value timev = ACE_High_Res_Timer::gettimeofday_hr();
    return double(timev.sec()) + timev.usec() * 1e-6; 
#else
    // on other operating systems, high res seems dysfunctional
    // which is weird since ACE manual claims it maps naturally 
    // on gettimoday...
    ACE_Time_Value timev = ACE_OS::gettimeofday ();
    return double(timev.sec()) + timev.usec() * 1e-6; 
#endif
}

void Time::turboBoost() {
#ifdef ACE_WIN32
    // only does something on Microsoft Windows
    TIMECAPS tm;
    timeGetDevCaps(&tm, sizeof(TIMECAPS));
    timeBeginPeriod(tm.wPeriodMin);
#endif
}

void Time::yield() {
    ACE_Time_Value tv(0);
    ACE_OS::sleep(tv);
}

