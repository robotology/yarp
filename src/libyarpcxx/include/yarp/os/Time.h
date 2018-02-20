/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPCXX_Time_INC
#define YARPCXX_Time_INC

#include <yarp/yarpcxx.h>

namespace yarpcxx {
    namespace os {
        class Time;
    }
}

class yarpcxx::os::Time {
public:
    static void delay(double seconds) {
        yarpTimeDelay(seconds);
    }

    static double now() {
        return yarpTimeNow();
    }

    static void yield() {
        yarpTimeYield();
    }

};

namespace yarp {
    using namespace yarpcxx;
}

#endif
