// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/NetworkClock.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/NestedContact.h>
#include <yarp/os/Network.h>

using namespace yarp::os;

NetworkClock::NetworkClock() {
    sec = 0;
    nsec = 0;
    t = 0;
}

bool NetworkClock::open(const ConstString& name) {
    port.setReadOnly();
    NestedContact nc(name);
    if (nc.getNestedName()=="") {
        Contact src = NetworkBase::queryName(name);
        if (src.isValid()) {
            bool ok = port.open("");
            if (!ok) return false;
            // we want always the last clock received
            // port.setStrict(false);
            return NetworkBase::connect(name,port.getName());
        } else {
            fprintf(stderr,"Cannot find time port \"%s\"; for a time topic specify \"%s@\"\n", name.c_str(), name.c_str());
            return false;
        }
    }
    return port.open(name);
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
        /**
         * if we ask to sleep less than 1E-12, return
         * this addresses a strange bug where we ask NetworkClock
         * to sleep for quantities around 1E-15
         * probably due to int to double conversion
         */
        c.delay(seconds - (now()-start) - 1E-12);
    } while (seconds - (now()-start) > 1E-12);
}

bool NetworkClock::isValid() const {
    return (sec!=0) || (nsec!=0);
}

