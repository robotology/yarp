// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/YARPTime.h>

#include <yarp/os/Time.h>

using namespace yarp::os;

double YARPTime::GetTimeAsSeconds() {
    return Time::now();
}

void YARPTime::DelayInSeconds(double delay_in_seconds) {
    Time::delay(delay_in_seconds);
}

