/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

// define the Target class
#include "TargetVer2.h"

int main() {
    Network yarp;

    Port p;            // Create a port.
    p.open("/target/raw/in");     // Give it a name on the network.
    Target t;                     // Make a place to store things.
    while (true) {
        p.read(t);         // Read from the port.  Waits until data arrives.
        // Do something with data.
        printf("Got (%d,%d)\n", t.x, t.y);
    }

    return 0;
}
