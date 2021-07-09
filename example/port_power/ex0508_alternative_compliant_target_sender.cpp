/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

// define the Target class
#include "TargetVer1b.h"

int main() {
    Network yarp;

    int ct = 0;
    Port p;            // Create a port.
    p.open("/target/raw/out");    // Give it a name on the network.
    while (true) {
        BinPortable<Target> b;        // Make a place to store things.
        b.content().x = ct;
        b.content().y = 42;
        ct++;
        p.write(b);      // Send the data.
        printf("Sent (%d,%d)\n", b.content().x, b.content().y);
        Time::delay(1);
    }

    return 0;
}
