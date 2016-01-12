// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_CLOCKTIME
#define YARP2_CLOCKTIME

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class Clock;
    }
}

class YARP_OS_API yarp::os::Clock {
public:
    /**
     *
     * Destructor.
     *
     */
    virtual ~Clock() {}


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
     *
     * Check if time is valid (non-zero).  For example, for
     * a network clock, if no timestamp has yet been received, 
     * this method will return false.
     *
     */
    virtual bool isValid() const = 0;
};

#endif
