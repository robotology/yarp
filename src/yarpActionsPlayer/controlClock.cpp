/*
 * SPDX-FileCopyrightText: 2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "controlClock.h"
#include <yarp/os/Time.h>

double ControlClock::getElapsedTime()
{
    if (running)
    {
        return elapsedTime + (yarp::os::Time::now() - startTime);
    }
    return elapsedTime;
}

void ControlClock::startTimer()
{
    if (!running)
    {
        startTime = yarp::os::Time::now();
        running = true;
    }
}

void ControlClock::pauseTimer()
{
    if (running)
    {
        elapsedTime += (yarp::os::Time::now() - startTime);
        running = false;
    }
}
void ControlClock::resetTimer()
{
    running = false;
    startTime = 0.0;
    elapsedTime = 0.0;
}
