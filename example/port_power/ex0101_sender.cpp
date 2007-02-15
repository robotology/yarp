// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

int main() {
    Network::init();
    
    int ct = 0;
    BufferedPort<Bottle> p; // Create a port.
    p.open("/out");         // Give it a name on the network.
    while (true) {
        Bottle& b = p.prepare(); // Get a place to store things.
        b.clear();  // clear is important - b might be a reused object
        b.add("hello");
        b.add("world");
        b.add(ct);
        ct++;
        printf("Sending %s\n", b.toString().c_str());
        p.write();            // Send the data.
        // after write() is called, user should not touch b.
        Time::delay(1);
    }

    Network::fini();
    return 0;
}
