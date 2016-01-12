/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_TIME_H
#define YARP_OS_TIME_H

#include <yarp/os/ConstString.h>
#include <yarp/os/Clock.h>

namespace yarp {
    namespace os {
        class Time;
    }
}

/**
 * \ingroup key_class
 *
 * Services related to time -- delay, current time.
 */
class YARP_OS_API yarp::os::Time {
public:
    /**
     * Wait for a certain number of seconds.  This may be fractional.
     * @param seconds the duration of the delay, in seconds
     */
    static void delay(double seconds);

    /**
     * Return the current time in seconds, relative to an arbitrary
     * starting point.
     * @return the time in seconds
     */
    static double now();

    /**
     * The calling thread releases its remaining quantum upon calling
     * this function.
     */
    static void yield();

    /**
     * For OS where it makes sense sets the scheduler to be called more often.
     * This sets the scheduler to be run to the maximum possible rate based
     * on the capability of the hardware.
     * Specifically, on Microsoft Windows, high resolution scheduling is
     * used.
     */
    static void turboBoost();

    /**
     *
     * Configure YARP to use system time (this is the default).
     *
     */
    static void useSystemClock();

    /**
     *
     * Configure YARP to read time from a specified topic.  The
     * same effect can also be achieved using the YARP_CLOCK
     * environment variable.  Topic should provide two integers,
     * time in seconds followed by residual in nanoseconds.
     * If yarp is configured according to \ref yarp_with_ros,
     * then ROS /clock topic will work.
     *
     * \see yarp::os::NetworkClock
     *
     */
    static void useNetworkClock(const ConstString& clock);

    /**
     *
     * Provide a custom time source.
     *
     */
    static void useCustomClock(Clock *clock);

    /**
     *
     * Check if YARP is providing system time.
     *
     */
    static bool isSystemClock();

    /**
     *
     * Check if time is valid (non-zero).  If a network clock is
     * in use and no timestamp has yet been received, this
     * method will return false.
     *
     */
    static bool isValid();
};

#endif // YARP_OS_TIME_H
