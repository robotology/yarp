/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006, 2011 Anne van Rossum <anne@almende.com>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Time.h>
#include <yarp/os/impl/PlatformTime.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/NetworkClock.h>
#include <yarp/os/Network.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/impl/TimeImpl.h>


#ifdef ACE_WIN32
// for WIN32 MM functions
#include <mmsystem.h>
#endif

using namespace yarp::os;
using namespace yarp::os::impl;

static bool clock_owned = false;
static bool network_clock_ok = false;
static Clock *pclock = nullptr;
static yarpClockType yarp_clock_type  = YARP_CLOCK_UNINITIALIZED;

static void lock() {
    yarp::os::impl::ThreadImpl::timeMutex->wait();
}

static void unlock() {
    yarp::os::impl::ThreadImpl::timeMutex->post();
}

void printNoClock_ErrorMessage()
{
    YARP_ERROR(Logger::get(), "\n Warning an issue has been found, please update the code.\n \
    Clock is not initialized: This means YARP framework has not been properly initialized. \n \
    The clock can be initialized with one of the following methods:\n \
    - Create yarp::os::Network object or call yarp::os::Network::init()\n \
    - Call useSystemClock()\n \
    otherwise use yarp::os::SystemClock::nowSystem() and yarp::os::SystemClock::delaySystem() instead of Time::now() and Time::delay()\n");
}

static Clock *getClock()
{
    // if no clock was ever set, fallback to System Clock
    if(pclock == nullptr)
    {
        /*
         * Assuming this should never happen, if we do get here, what shall be done??
         *
         * 1: create system clock
         *   If we get here, probably there is some sort of race condition while changing the clock,
         *   so creating a system clock may not be what we want to do, and this may interfere with the
         *   clock really wanted by the user, i.e. this system clock may be destroyed again to
         *   instantiate the good one, leaving space for another possible race condition.
         *
         * 2: use the system clock only for this call
         *
         * 3: exit now and ask user to correctly initialize the framework
         *   This is better because it shows initialization problems right from the start and help user
         *   to fix the code, which may otherwise lead to undefined behaviour.
         *
         * So now initialize a system clock and go forward for backward compatibility, if YARP_NO_DEPRECATED
         * is true, exit now! This is gonna be the default in the next release.
         */
        printNoClock_ErrorMessage();

#ifdef YARP_NO_DEPRECATED  // for back compatibility
        ::exit(-1);
#endif
    }
    return pclock;
}

void yarp::os::impl::removeClock()
{
    if(pclock) {
        delete pclock;
        pclock = nullptr;
    }
    yarp_clock_type = YARP_CLOCK_UNINITIALIZED;
}

void Time::delay(double seconds) {
    if(isSystemClock())
        return SystemClock::delaySystem(seconds);

    Clock *clk = getClock();
    clk->delay(seconds);
}

double Time::now() {
    if(isSystemClock())
        return SystemClock::nowSystem();

    Clock *clk = getClock();
    return clk->now();
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
    return yarp::os::Thread::yield();
}


void Time::useSystemClock()
{
#ifdef YARP_NO_DEPRECATED
    if(!isSystemClock())
#endif
    {
        lock();

        Clock *old_pclock = pclock;
        bool old_clock_owned = clock_owned;

        pclock = new SystemClock();
        yAssert(pclock);
        yarp_clock_type = YARP_CLOCK_SYSTEM;
        clock_owned = true;

        if(old_clock_owned && old_pclock)
            delete old_pclock;

        unlock();
    }

}

/* Creation of network clock may fail for different causes:
 * - cannot open port
 * - cannot connect to nameserver
 *
 * They may be handled in different ways, for example for the firsts two cases, it simply fails and
 * continue with system clock. Failure should be reported to the user and 'pclock' pointer will be temporary
 * set to NULL (which is an INVALID value).
 * isSystemClock() will still return true because it is the clock currently active.
 *
 * In case the source clock is not yet publishing time data, we wait here for the first valid clock, this way
 * the application will not start until the clock is correctly configured.
 * In this situation
 * - isSystemClock()    will be false
 * - isNetworkClock()   will be true
 * - isValid()          will be false until the first clock mesage is received, then it'll be true
 *
 * As soon as the clock starts beeing published, the networkClock has to acknowledge it and 'attach' to it. Clock will
 * then be valid.
 */
void Time::useNetworkClock(const ConstString& clock, ConstString localPortName)
{
    // re-create the clock also in case we already use a network clock, because
    // the input clock port may be different or the clock producer may be changed (different
    // clock source publishing on the same port/topic), so we may need to reconnect.
    lock();

    Clock *old_pclock = pclock;   // store current clock pointer to delete it afterward
    bool old_clock_owned = clock_owned;
    NetworkClock *_networkClock = new NetworkClock();
    if(_networkClock == nullptr)
    {
        YARP_FAIL(Logger::get(), "failed creating NetworkClock client");
        return;
    }
    else
    {
        if (_networkClock->open(clock, localPortName))
        {
            network_clock_ok = true;    // see if it is really needed
            // updating clock pointer with the new one already initialized.

            pclock = _networkClock;
            clock_owned = true;
            yarp_clock_type = YARP_CLOCK_NETWORK;
        }
        else
        {
            YARP_FAIL(Logger::get(), "failed creating NetworkClock client, cannot open input port");
            return;
        }
    }

    if(old_clock_owned && old_pclock)
        delete old_pclock;
    unlock();

    int i=-1;
    while(pclock && !pclock->isValid() )
    {
        i++;
        if((i%50) == 0)
        {
            YARP_INFO(Logger::get(), "Waiting for clock server to start broadcasting data ...");
            i=0;
        }
        SystemClock::delaySystem(0.1);
    }
}

void Time::useCustomClock(Clock *clock) {
    if(clock == nullptr) {
        YARP_FAIL(Logger::get(), "failed configuring CustomClock client");
        return;
    }

    if(!clock->isValid()) {
        YARP_FAIL(Logger::get(), "Error: CustomClock is not valid");
        return;
    }

    lock();

    // store current clock pointer to delete it afterward
    Clock *old_pclock = pclock;
    bool old_clock_owned = clock_owned;

    pclock = clock;
    yarp_clock_type = YARP_CLOCK_CUSTOM;
    clock_owned = false;

    // delete old clock
    if(old_clock_owned && old_pclock)
        delete old_pclock;

    unlock();
}

bool Time::isSystemClock() {
#ifdef YARP_NO_DEPRECATED
    return (yarp_clock_type==YARP_CLOCK_SYSTEM);
#else
    if(yarp_clock_type==YARP_CLOCK_UNINITIALIZED)
    {
        printNoClock_ErrorMessage();
        yarp_clock_type=YARP_CLOCK_SYSTEM;
    }
    return (yarp_clock_type==YARP_CLOCK_SYSTEM);
#endif
}

bool Time::isNetworkClock() {
    return (yarp_clock_type==YARP_CLOCK_NETWORK);
}

bool Time::isCustomClock() {
    return (yarp_clock_type==YARP_CLOCK_CUSTOM);
}

yarpClockType Time::getClockType()
{
    return yarp_clock_type;
}

yarp::os::ConstString Time::clockTypeToString(yarpClockType type)
{
    yarp::os::ConstString clockTypeString("");
    if(type == -1)
        type = yarp_clock_type;

    switch(type)
    {
        case YARP_CLOCK_SYSTEM:
            clockTypeString = "System clock";
            break;

        case YARP_CLOCK_NETWORK:
            clockTypeString = "Network clock";
            break;

        case YARP_CLOCK_CUSTOM:
            clockTypeString = "Custom clock";
            break;

        case YARP_CLOCK_UNINITIALIZED:
            clockTypeString = "Clock has not been initialized yet: This should never happen. Is the object yarp::os::Network been initialized?";
            break;

        default:
            clockTypeString = "Unknown clock: This should never happen. Is the object yarp::os::Network been initialized?";
            break;
    }
    return clockTypeString;
}


bool Time::isValid()
{
#ifndef YARP_NO_DEPRECATED
    if( (yarp_clock_type == YARP_CLOCK_SYSTEM) || (yarp_clock_type==YARP_CLOCK_UNINITIALIZED) )
        return true;
    else
#endif
    // The clock must never be NULL here (when YARP_NO_DEPRECATED is true)
    return getClock()->isValid();
}
