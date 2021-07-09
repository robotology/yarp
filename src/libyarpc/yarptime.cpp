/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
