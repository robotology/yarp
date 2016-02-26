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
    Port p;            // Create a port.
    p.open("/out");    // Give it a name on the network.
    while (true) {
        Bottle in,out;   // Make places to store things.
        // prepare command "out".
        out.clear();
        out.add("hello");
        out.add("world");
        out.add(ct);
        ct++;
        p.write(out,in); // send command, wait for reply.
        // process response "in".
        if (in.size()>0) {
            printf("Got response: %s\n", in.toString().c_str());
        } else { 
            printf("No response\n");
        }
        Time::delay(1);
    }

    return 0;
}



