/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_TIME_H
#define YARP_OS_TIME_H

#include <yarp/os/ConstString.h>
#include <yarp/os/Clock.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/NetworkClock.h>

namespace yarp {
    namespace os {
        class Time;

        typedef enum {
            YARP_CLOCK_UNINITIALIZED=-1,
            YARP_CLOCK_DEFAULT,
            YARP_CLOCK_SYSTEM,
            YARP_CLOCK_NETWORK,
            YARP_CLOCK_CUSTOM
        } yarpClockType;
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
     * return true on success, false on failure. Possible causes of
     * failure are invalid port name or address conflict.
     *
     * Throws assert in case of failure
     */
    static void useNetworkClock(const ConstString& clock, ConstString localPortName="");

    /**
     *
     * Configure YARP clients to use a custom clock source provided by the
     * user. The Clock source must implement the yarp::os::Clock interface.
     * This function check clock->isValid() to verify the source is working
     * properly.
     *
     * Possible causes of failure are: clock pointer invalid or isValid() false.
     *
     * Throws assert in case of failure
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
     * Check if YARP is providing network time.
     *
     */
    static bool isNetworkClock();

    /**
     *
     * Check if YARP is using a user-defined custom time.
     *
     */
    static bool isCustomClock();

    /**
     * \return enum type with the current clock type used
     */
    static yarpClockType getClockType();

    /**
     *
     * Converts clock type enum into string.
     * @type Convert specified enum into string.
     *
     * clockTypeToString
     */
     static yarp::os::ConstString clockTypeToString(yarpClockType type);

    /**
     *
     * Check if time is valid (non-zero).  If a network clock is
     * in use and no timestamp has yet been received, this
     * method will return false.
     *
     */
    static bool isValid();

private:
    static yarpClockType yarp_clock_type;

};

#endif // YARP_OS_TIME_H
