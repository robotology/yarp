/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>

#include "yarp.h"
#include "yarpimpl.h"

YARP_DEFINE(void) yarpTimeDelay(double seconds) {
    Time::delay(seconds);
}

YARP_DEFINE(double) yarpTimeNow() {
    return Time::now();
}


YARP_DEFINE(void) yarpTimeYield() {
    Time::yield();
}
