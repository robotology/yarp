/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_CLOCK_H
#define YARP_OS_CLOCK_H

#include <yarp/os/api.h>

namespace yarp {
namespace os {

class YARP_os_API Clock
{
public:
    /**
     * Destructor.
     */
    virtual ~Clock();

    /**
     * Return the current time in seconds, relative to an arbitrary
     * starting point.
     * @return the time in seconds
     */
    virtual double now() = 0;

    /**
     * Wait for a certain number of seconds.  This may be fractional.
     * @param seconds the duration of the delay, in seconds
     */
    virtual void delay(double seconds) = 0;

    /**
     * Check if time is valid (non-zero).  For example, for
     * a network clock, if no timestamp has yet been received,
     * this method will return false.
     */
    virtual bool isValid() const = 0;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_CLOCK_H
