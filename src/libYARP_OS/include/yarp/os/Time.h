// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_TIME_
#define _YARP2_TIME_

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
class yarp::os::Time {
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

};

#endif
