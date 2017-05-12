/*
 * Copyright (C) 2006, 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/Time.h>
#include <yarp/os/impl/PlatformTime.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/NetworkClock.h>
#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/impl/ThreadImpl.h>

#ifdef ACE_WIN32
// for WIN32 MM functions
#include <mmsystem.h>
#endif

using namespace yarp::os;

static Clock *pclock = YARP_NULLPTR;
static bool clock_owned = false;
static ConstString *network_clock_name = YARP_NULLPTR;
static bool network_clock_ok = false;

static yarpClockType yarp_clock_type  = UNDEFINED_CLOCK;

static void lock()
{
    yarp::os::impl::ThreadImpl::timeMutex->wait();
}

static void unlock()
{
    yarp::os::impl::ThreadImpl::timeMutex->post();
}

static Clock *createsystemClock()
{
    yarp_clock_type = SYSTEM_CLOCK;
    pclock = new SystemClock();
    return pclock;
}

/* Creation of network clock may fail for different causes:
 * - cannot open port
 * - cannot connect to nameserver
 * - clock has not been created yet.
 * 
 * They may be handled in different ways, for example for the firsts two cases, it simply fails and
 * continue with system clock. Failure should be reported to the user and 'pclock' pointer will be temporary 
 * set to NULL (which is an INVALID value).
 * isSystemClock() will still return true because it is the clock currently active.
 * 
 * Vice versa, if the port was opened, but the clock is not published yet, then isSystemClock() will return false,
 * 'pclock' pointer will be different from NULL, isValid() is false.
 * As soon as the clock start beeing published, the networkClock has to acknowledge it and 'attach' to it. Clock will
 * then be valid.
 * Calling a removeClock in this middle-step, will destroy the network clock and remove it from the 'waiting for 
 * clock' state. By default no clock will be used as a substitute.
 * 
 * When getClock() is called, and no clock has been set, the systemClock will be created and used.
 */
static Clock *createNetworkClock()
{   
    // this is a private function, no need for lock(), mutex shall be used by caller

    // Create the new network clock
    NetworkClock *_networkClock = NULL;
    Clock *old_pclock;
    ConstString name("/clock");    // use '/clock' as a default port name for source clock

    if (network_clock_name)
        name = *network_clock_name;

    _networkClock = new NetworkClock();
    yAssert(_networkClock);

    if (_networkClock)
    {
        if (_networkClock->open(name))
        {
            network_clock_ok      = true;    // see if it is really needed
            // updating clock pointer with the new one already initialized.
            old_pclock = pclock;
            pclock = _networkClock;
            clock_owned = true;
            yarp_clock_type = NETWORK_CLOCK;
        }
    }
    else
    {
        yError() << "Failed creating network clock... fallback to system clock";
        // _networkClock = createsystemClock();
        // maybe just set the pclock here and quit? what about cleaning old clock?
    }

    /* wait for the network clock to get a valid value on port before start using it?
     * If the clock is not published yet, this means that only the thread calling the
     * useNetworkClock will wait, while other threads will keep running with the systemClock.
     *
     * In case we set the network clock as the current clock, and no one is publishing yet,
     * then all threads will get bloked waiting for the clock. I think this is more safe
     * then having part of the code using a clock and another part using a different clock.
     */

    // Remove old clock
    if (pclock != NULL)
    {
        delete old_pclock;          // The clock must take care of wake up all sleeping threads
    }
    else
    {
        // pclock should never be NULL
        yDebug() << "Got a previous null pclock pointer while setting network clock. This is not harmful but should never happen.";
    }
    return pclock;
}


// To be called ONLY INSIDE a function which sets a new clock.
// Never leave the clock pointer set to NULL
void Time::removeClock()
{
    // make the current clock invalid before destroying it so new request will already
    // be redirected to the system clock.
    // Here we are already inside ::lock()

    lock();
    Clock *old_pclock = pclock;
    bool old_clock_owned = clock_owned;
    pclock = NULL;
    clock_owned = false;

    if (network_clock_name) delete network_clock_name;

    network_clock_name = NULL;

    if( (old_pclock) && (yarp_clock_type != CUSTOM_CLOCK) )
    {
//         if (old_clock_owned)
//         {
            delete old_pclock;          // This will wake up all sleeping threads
            old_clock_owned = false;
//         }

        old_pclock = NULL;
    }
    unlock();
}

static Clock *getClock()
{
    // if no clock was ever set, fallback to System Clock
    if(pclock == NULL)
    {
        /* WIP
         * Assuming this should never happen, if we do get here, what shall be done??
         *
         * 1: create system clock
         *   If we get here, probably there is some sort of race condition while changing the clock,
         *   so creating a system clock may not be what we want to do, and this may interfere with the
         *   clock really wanted by the user, i.e. this system clock may be destroyed again to
         *   instantiate the good one, leaving space for another possible race condition.
         * 2: use the system clock only for this call
         */
        pclock = createsystemClock();
    }
    return pclock;
}

void Time::delay(double seconds)
{
    if(isSystemClock())
        return SystemClock::delaySystem(seconds);

    Clock *clk = getClock();
    clk->delay(seconds);
}

double Time::now()
{
    if(isSystemClock())
        return SystemClock::nowSystem();

    Clock *clk = getClock();
    return clk->now();
}

void Time::turboBoost()
{
#ifdef ACE_WIN32
    // only does something on Microsoft Windows
    TIMECAPS tm;
    timeGetDevCaps(&tm, sizeof(TIMECAPS));
    timeBeginPeriod(tm.wPeriodMin);
#endif
}

void Time::yield()
{
#ifdef YARP_HAS_ACE
    ACE_Time_Value tv(0);
    ACE_OS::sleep(tv);
#else
    sleep(0);
#endif
}


void Time::useSystemClock()
{
    if(!isSystemClock())
    {
        removeClock();
        lock();
        pclock = createsystemClock();
        unlock();
    }
}

void Time::useNetworkClock(const ConstString &clock)
{
    // re-create the clock also in case we already use a network clock, because
    // the input clock port may be different or the clock producer may be changed
    // so we need to reconnect. (Using persistant connection this may not be required).
    network_clock_name = new ConstString(clock);
    removeClock();
    lock();
    pclock = createNetworkClock();
    unlock();
    isValid();
}

void Time::useCustomClock(Clock *clock)
{
    if(clock == NULL)
        yError("useCustomClock called with NULL clock, cannot proceed.");
    yAssert(clock);
    
    if(!clock->isValid())
        yError("useCustomClock called with invalid clock, cannot proceed.");
    yAssert(clock);
            
    removeClock();
    lock();
    pclock = clock;
    yarp_clock_type = CUSTOM_CLOCK;
    unlock();
}

bool Time::isSystemClock()
{
    return (yarp_clock_type==SYSTEM_CLOCK);
}

bool Time::isNetworkClock()
{
    return (yarp_clock_type==NETWORK_CLOCK);
}

bool Time::isCustomClock()
{
    return (yarp_clock_type==CUSTOM_CLOCK);
}

yarp::os::ConstString Time::getClockType(yarpClockType type)
{
    yarp::os::ConstString clockTypeString("");
    if(type == -1)
        type = yarp_clock_type;

    switch(type)
    {
        case SYSTEM_CLOCK:
        {
            clockTypeString = "System clock";
        }
        break;

        case NETWORK_CLOCK:
        {
            clockTypeString = "Network clock";
        }
        break;

        case CUSTOM_CLOCK:
        {
            clockTypeString = "Custom clock";
        }
        break;

        case UNDEFINED_CLOCK:
        {
            clockTypeString = "Undefined clock (this should never happen)";
        }
        break;

        default:
        {
            clockTypeString = "Unknown clock (this should never happen)";
        }
    }
    return clockTypeString;
}


bool Time::isValid()
{
    // The clock should never be NULL here because getClock creates it if NULL... 
    // possible race condition between getClock and removeClock??
    return getClock()->isValid();
}
