/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_SYSTEMCLOCK_H
#define YARP_OS_SYSTEMCLOCK_H

#include <yarp/os/Clock.h>

namespace yarp {
    namespace os {
        class SystemClock;
    }
}


class YARP_OS_API yarp::os::SystemClock : public Clock {
public:
    virtual double now() override {
        return nowSystem();
    }

    virtual void delay(double seconds) override {
        delaySystem(seconds);
    }

    virtual bool isValid() const override { return true; }

    static double nowSystem();
    static void delaySystem(double seconds);

};


#endif // YARP_OS_SYSTEMCLOCK_H
