// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/NetworkClock.h>
#include <yarp/os/SystemClock.h>

using namespace yarp::os;

NetworkClock::NetworkClock() {
    sec = 0;
    nsec = 0;
    t = 0;
}

bool NetworkClock::open(const ConstString& name) {
    port.setReadOnly();
    return port.open(name + "@");
}

double NetworkClock::now() {
    Bottle *bot = port.read(false);
    if (bot) {
        sec = bot->get(0).asInt();
        nsec = bot->get(1).asInt();
        t = sec + (nsec*1e-9);
    }
    return t;
}

void NetworkClock::delay(double seconds) {
    if (seconds<=0) {
        return;
    }
    SystemClock c;
    double start = now();
    do {
        c.delay(1e-3);
    } while (now()-start<seconds);
}

bool NetworkClock::isValid() const {
    return (sec!=0) || (nsec!=0);
}

