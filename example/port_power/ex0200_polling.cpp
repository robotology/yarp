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

    BufferedPort<Bottle> p; // Create a port.
    p.open("/in");          // Give it a name on the network.
    while (true) {
        Bottle *b = p.read(false); // Read from the port.  Don't wait
        if (b!=NULL) {
            printf("Got %s\n", b->toString().c_str());
        } else {
            printf("No data yet...\n");
            Time::delay(0.5);
        }
    }

    return 0;
}
