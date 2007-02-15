// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

// define the Target class
#include "TargetVer2.h"

int main() {
    Network::init();
    
    Port p;            // Create a port.
    p.open("/target/raw/in");     // Give it a name on the network.
    Target t;                     // Make a place to store things.
    while (true) {
        p.read(t);         // Read from the port.  Waits until data arrives.
        // Do something with data.
        printf("Got (%d,%d)\n", t.x, t.y);
    }
    
    Network::fini();
    return 0;
}
