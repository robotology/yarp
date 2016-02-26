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
