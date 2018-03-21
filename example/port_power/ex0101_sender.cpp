/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
        b.addString("hello");
        b.addString("world");
        b.addInt(ct);
        ct++;
        printf("Sending %s\n", b.toString().c_str());
        p.write();            // Send the data.
        // after write() is called, user should not touch b.
        Time::delay(1);
    }

    return 0;
}
