/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

int main() {
    Network yarp;
    
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

    return 0;
}
