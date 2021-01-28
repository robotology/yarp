/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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
    Port p;            // Create a port.
    p.open("/out");    // Give it a name on the network.
    while (true) {
        Bottle b;        // Make a place to store things.
        b.clear();
        b.addString("hello");
        b.addString("world");
        b.addInt32(ct);
        ct++;
        p.write(b);      // Send the data.
        printf("Sent %s\n", b.toString().c_str());
        Time::delay(1);
    }

    return 0;
}
