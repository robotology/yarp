// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/dev/PolyDriver.h>

using namespace yarp::dev;

bool PolyDriver::open(const char *txt) {
    if (isValid()) {
        // already open - should close first
        return false;
    }
    dd = Drivers::factory().open(txt);
    return isValid();
}


bool PolyDriver::open(yarp::os::Searchable& config) {
    if (isValid()) {
        // already open - should close first
        return false;
    }
    dd = Drivers::factory().open(config);
    return isValid();
}
