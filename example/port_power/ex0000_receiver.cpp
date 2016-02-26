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
