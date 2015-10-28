// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Time.h>
#include <yarp/os/impl/PlatformTime.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/NetworkClock.h>
#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <yarp/os/impl/ThreadImpl.h>

#ifdef ACE_WIN32
// for WIN32 MM functions
#include <mmsystem.h>
#endif

using namespace yarp::os;

static Clock *pclock = NULL;
static bool clock_owned = false;
static ConstString *network_clock_name = NULL;
static bool network_clock_pending = false;

static void lock() {
    yarp::os::impl::ThreadImpl::timeMutex->wait();
}

static void unlock() {
    yarp::os::impl::ThreadImpl::timeMutex->post();
}

static void removeClock()
{
    Clock *old_pclock = pclock;
    bool old_clock_owned = clock_owned;

    // make the current clock invalid before destroying it so new request will already
    // be redirected to the system clock.
    // Here we are already inside ::lock()
    pclock = NULL;
    clock_owned = false;

    if (network_clock_name) delete network_clock_name;
    network_clock_name = NULL;
    network_clock_pending = false;

    if (old_pclock) {
        if (old_clock_owned) {
            delete old_pclock;          // This will wake up all sleeping threads
            old_clock_owned = false;
        }
        old_pclock = NULL;
    }
}

static Clock *getClock() {
    if (network_clock_pending) {
        ConstString name;
        NetworkClock *nc = NULL;
        lock();
        if (network_clock_pending) {
            name = "";
            if (network_clock_name) name = *network_clock_name;
            removeClock();
            network_clock_pending = false;
            pclock = nc = new NetworkClock();
            clock_owned = true;
            yAssert(pclock);
        }
        unlock();
        if (nc) {
            nc->open(name);
        }
    }
    return pclock;
}

void Time::delay(double seconds) {
    Clock *clk = getClock();
    if (clk) {
        clk->delay(seconds);
    } else {
        SystemClock::delaySystem(seconds);
    }
}

double Time::now() {
    Clock *clk = getClock();
    if (clk) return clk->now();
    return SystemClock::nowSystem();
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
#ifdef YARP_HAS_ACE
    ACE_Time_Value tv(0);
    ACE_OS::sleep(tv);
#else
    sleep(0);
#endif
}


void Time::useSystemClock() {
    lock();
    removeClock();
    unlock();
}

void Time::useNetworkClock(const ConstString& clock) {
    lock();
    removeClock();
    network_clock_name = new ConstString(clock);
    network_clock_pending = true;
    unlock();
}

void Time::useCustomClock(Clock *clock) {
    lock();
    removeClock();
    pclock = clock;
    yAssert(pclock);
    unlock();
}

bool Time::isSystemClock() {
    return (pclock==NULL);
}

bool Time::isValid() {
    Clock *clk = getClock();
    if (clk) return clk->isValid();
    return true;
}
