// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SYSTEMCLOCKTIME_
#define _YARP2_SYSTEMCLOCKTIME_

#include <yarp/os/Clock.h>

namespace yarp {
    namespace os {
        class SystemClock;
    }
}


class YARP_OS_API yarp::os::SystemClock : public Clock {
public:
    virtual double now() { 
        return nowSystem(); 
    }

    virtual void delay(double seconds) {
        delaySystem(seconds);
    }

    virtual bool isValid() const { return true; }

    static double nowSystem();
    static void delaySystem(double seconds);

};


#endif
