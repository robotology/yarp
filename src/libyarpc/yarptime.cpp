// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

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

