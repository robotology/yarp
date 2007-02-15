// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

// define the Target class
#include "TargetVer3.h"

int main() {
    Network::init();
    
    int ct = 0;
    Port p;            // Create a port.
    p.open("/out");    // Give it a name on the network.
    while (true) {
        Target t;      // Make a place to store things.
        t.x = ct;
        t.y = 42;
        ct++;
        p.write(t);    // Send the data.
        printf("Sent (%d,%d)\n", t.x, t.y);
        Time::delay(1);
    }

    Network::fini();
    return 0;
}
