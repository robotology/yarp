/*
* Copyright (C) 2009 The RobotCub consortium
* Author: Lorenzo Natale, Anne van Rossum, Paul Fitzpatrick
* Based on code by Paul Fitzpatrick 2007.
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/


#include <yarp/os/impl/PlatformTime.h>

#include <yarp/os/Time.h>

//time helper functions
void yarp::os::impl::getTime(YARP_timeval& now) {
    if (Time::isSystemClock()) {
#ifdef YARP_HAS_ACE
        now = ACE_OS::gettimeofday();
#else
        struct timezone *tz = nullptr;
        gettimeofday(&now, tz);
#endif
    } else {
#ifdef YARP_HAS_ACE
        now.set(Time::now());
#else
        double t = Time::now();
        now.tv_sec = static_cast<int>(t);
        now.tv_usec = static_cast<int>((t-now.tv_sec)*1e6);
#endif
    }
}


void yarp::os::impl::sleepThread(YARP_timeval& sleep_period) {
    if (Time::isSystemClock()) {
#ifdef YARP_HAS_ACE
        if (sleep_period.usec() < 0 || sleep_period.sec() < 0)
            sleep_period.set(0, 0);
        ACE_OS::sleep(sleep_period);
#else
        if (sleep_period.tv_usec < 0 || sleep_period.tv_sec < 0) {
            sleep_period.tv_usec = 0;
            sleep_period.tv_sec = 0;
        }
        usleep(sleep_period.tv_sec * 1000000 + sleep_period.tv_usec );
#endif
    } else {
        Time::delay(toDouble(sleep_period));
    }
}


void yarp::os::impl::addTime(YARP_timeval& val, const YARP_timeval & add) {
#ifdef YARP_HAS_ACE
    val += add;
#else
    val.tv_usec += add.tv_usec;
    int over = val.tv_usec % 1000000;
    if (over != val.tv_usec) {
        val.tv_usec = over;
        val.tv_sec++;
    }
    val.tv_sec += add.tv_sec;
#endif
}


void yarp::os::impl::subtractTime(YARP_timeval & val, const YARP_timeval & subtract) {
#ifdef YARP_HAS_ACE
    val -= subtract;
#else
    if (val.tv_usec > subtract.tv_usec) {
        val.tv_usec -= subtract.tv_usec;
        val.tv_sec -= subtract.tv_sec;
        return;
    }
    int over = 1000000 + val.tv_usec - subtract.tv_usec;
    val.tv_usec = over;
    val.tv_sec--;
    val.tv_sec -= subtract.tv_sec;
#endif
}


double yarp::os::impl::toDouble(const YARP_timeval &v) {
#ifdef YARP_HAS_ACE
    return double(v.sec()) + v.usec() * 1e-6;
#else
    return double(v.tv_sec) + v.tv_usec * 1e-6;
#endif
}


void yarp::os::impl::fromDouble(YARP_timeval &v, double x, int unit) {
#ifdef YARP_HAS_ACE
        v.msec(static_cast<int>(x*1000/unit+0.5));
#else
        v.tv_usec = static_cast<int>(x*1000000/unit+0.5) % 1000000;
        v.tv_sec = static_cast<int>(x/unit);
#endif
}
