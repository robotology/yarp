/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_TIME_H
#define YARP_OS_TIME_H

#include <yarp/os/Clock.h>
#include <yarp/os/NetworkClock.h>
#include <yarp/os/SystemClock.h>

#include <string>

namespace yarp {
namespace os {

enum class ShouldUseSystemClock
{
    No = 0,
    Yes = 1
};

enum yarpClockType
{
    YARP_CLOCK_UNINITIALIZED = -1,
    YARP_CLOCK_DEFAULT,
    YARP_CLOCK_SYSTEM,
    YARP_CLOCK_NETWORK,
    YARP_CLOCK_CUSTOM
};

/*
 * Services related to time -- delay, current time.
 */
namespace Time {

/**
 * Wait for a certain number of seconds.  This may be fractional.
 * @param seconds the duration of the delay, in seconds
 */
YARP_os_API void delay(double seconds);

/**
 * Return the current time in seconds, relative to an arbitrary
 * starting point.
 * @return the time in seconds
 */
YARP_os_API double now();

/**
 * The calling thread releases its remaining quantum upon calling
 * this function.
 */
YARP_os_API void yield();

/**
 * Configure YARP to use system time (this is the default).
 */
YARP_os_API void useSystemClock();

/**
 * Configure YARP to read time from a specified topic.  The
 * same effect can also be achieved using the YARP_CLOCK
 * environment variable.  Topic should provide two integers,
 * time in seconds followed by residual in nanoseconds.
 * If yarp is configured according to @ref yarp_with_ros,
 * then ROS /clock topic will work.
 *
 * @see yarp::os::NetworkClock
 *
 * return true on success, false on failure. Possible causes of
 * failure are invalid port name or address conflict.
 *
 * Throws assert in case of failure
 */
YARP_os_API void useNetworkClock(const std::string& clock, const std::string& localPortName = "");

/**
 * Configure YARP clients to use a custom clock source provided by the
 * user. The Clock source must implement the yarp::os::Clock interface.
 * This function check clock->isValid() to verify the source is working
 * properly.
 *
 * Possible causes of failure are: clock pointer invalid or isValid() false.
 *
 * Throws assert in case of failure
 */
YARP_os_API void useCustomClock(Clock* clock);

/**
 * Check if YARP clock is initialized.
 */
YARP_os_API bool isClockInitialized();

/**
 * Check if YARP is providing system time.
 */
YARP_os_API bool isSystemClock();

/**
 * Check if YARP is providing network time.
 */
YARP_os_API bool isNetworkClock();

/**
 * Check if YARP is using a user-defined custom time.
 */
YARP_os_API bool isCustomClock();

/**
 * @return enum type with the current clock type used
 */
YARP_os_API yarpClockType getClockType();

/**
 * Converts clock type enum into string.
 * @param type Convert specified enum into string.
 */
YARP_os_API std::string clockTypeToString(yarpClockType type);

/**
 * Check if time is valid (non-zero).  If a network clock is
 * in use and no timestamp has yet been received, this
 * method will return false.
 */
YARP_os_API bool isValid();

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
/**
 * For OS where it makes sense sets the scheduler to be called more often.
 * This sets the scheduler to be run to the maximum possible rate based
 * on the capability of the hardware.
 * Specifically, on Microsoft Windows, high resolution scheduling is
 * used.
 *
 * @deprecated Since YARP 3.0.0
 */
YARP_DEPRECATED
YARP_os_API void turboBoost();
#endif

} // namespace Time
} // namespace os
} // namespace yarp

#endif // YARP_OS_TIME_H
