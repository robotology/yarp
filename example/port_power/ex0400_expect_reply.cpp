// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

int main() {
    Network::init();
    
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

    Network::fini();
}



