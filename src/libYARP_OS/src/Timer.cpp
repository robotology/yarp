/*
* Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
* All rights reserved.
*
* This software may be modified and distributed under the terms of the
* BSD-3-Clause license. See the accompanying LICENSE file for details.
*/

#include <yarp/os/Timer.h>
#include <yarp/os/Time.h>
#include <cmath>
using namespace yarp::os;
class Timer::Private
{
    typedef yarp::os::Timer::TimerCallback TimerCallback;
public:
    Private(TimerSettings sett, TimerCallback call, yarp::os::Mutex* mutex = nullptr) : m_settings(sett), m_callback(call), m_mutex(mutex){}
    TimerCallback    m_callback{nullptr};
    double           m_startStamp{0.0},
                     m_lastReal{0.0};
    unsigned int     m_runCount{0};
    yarp::os::Mutex* m_mutex;
    TimerSettings    m_settings;
};

Timer::Timer(const TimerSettings& settings, TimerCallback callback, Mutex* mutex) :
    RateThread(settings.rate.count()),
    impl(new Private(settings, callback, mutex)){}

void Timer::beforeStart()
{
    impl->m_startStamp = yarp::os::Time::now();
}

void Timer::run()
{
    if(getIterations() == 0)
    {
        yarp::os::Time::delay(getRate() * 0.001);
    }

    YarpTimerEvent event;

    event.currentReal     = yarp::os::Time::now();
    event.currentExpected = impl->m_startStamp + getIterations() * getRate() * 0.001;
    event.lastExpected    = event.currentExpected - getRate() * 0.001;
    event.lastReal        = impl->m_lastReal;
    event.lastDuration    = event.currentReal - impl->m_lastReal;
    event.runCount        = getIterations();

    if(impl->m_mutex) impl->m_mutex->lock();

    if(!impl->m_callback(event)) askToStop();

    if(impl->m_mutex) impl->m_mutex->unlock();

    impl->m_lastReal = event.currentReal;

    if((impl->m_settings.totalRunCount != 0 && impl->m_settings.totalRunCount >= getIterations()+1) ||
       (impl->m_settings.totalTime > 0.00001 && fabs(impl->m_settings.totalTime - impl->m_startStamp) > impl->m_settings.tollerance ))
    {
        askToStop();
    }
}

void Timer::setSettings(const TimerSettings& settings)
{
    impl->m_settings = settings;
}

const TimerSettings Timer::getSettings()
{
    return impl->m_settings;
}

Timer::~Timer()
{
    delete impl;
}
