// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

int main() {
    Network yarp;
    
    Port p;            // Create a port.
    p.open("/in");     // Give it a name on the network.
    Bottle b;          // Make a place to store things.
    while (true) {
        p.read(b);         // Read from the port.  Waits until data arrives.
        // Do something with data.
        printf("Got %s\n", b.toString().c_str());                
    }
    
    return 0;
}
