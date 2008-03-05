// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

int main() {
    Network yarp;
    
    int ct = 0;
    Port p;            // Create a port.
    p.open("/out");    // Give it a name on the network.
    while (true) {
        Bottle b;        // Make a place to store things.
        b.clear();
        b.add("hello");
        b.add("world");
        b.add(ct);
        ct++;
        p.write(b);      // Send the data.
        printf("Sent %s\n", b.toString().c_str());
        Time::delay(1);
    }

    return 0;
}
