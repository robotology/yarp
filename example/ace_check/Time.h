/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef _ACECHECK_TIME_
#define _ACECHECK_TIME_


/**
 * Services related to time -- delay, current time.
 */
class Time {
public:
    /**
     * Wait for a certain number of seconds.  This may be fractional.
     * @param seconds the duration of the delay, in seconds
     */
    static void delay(double seconds);

    /**
     * Return the current time in seconds, relative to an arbitrary 
     * starting point, using the best possible timer resolution.
     * @return the system time in seconds
     */
    static double now();

};

#endif
