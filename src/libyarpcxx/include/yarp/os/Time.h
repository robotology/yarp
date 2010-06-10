// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
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
