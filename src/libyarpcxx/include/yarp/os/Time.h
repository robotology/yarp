/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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
