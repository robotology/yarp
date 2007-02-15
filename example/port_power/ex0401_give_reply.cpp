// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

int main() {
    Network::init();

    Port p;            // Create a port.
    p.open("/in");     // Give it a name on the network.
    Bottle in, out;    // Make places to store things.
    while (true) {
        p.read(in,true); // Read from the port, warn that we'll be replying.
        printf("Got %s\n", in.toString().c_str());                
        out.clear();
        out.add("acknowledge");
        out.append(in);
        p.reply(out);    // send reply.
    }

    Network::fini();
}



