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

    Port p;            // Create a port.
    p.open("/in");     // Give it a name on the network.
    Bottle in, out;    // Make places to store things.
    while (true) {
        p.read(in,true); // Read from the port, warn that we'll be replying.
        printf("Got %s\n", in.toString().c_str());                
        out.clear();
        out.addString("acknowledge");
        out.append(in);
        p.reply(out);    // send reply.
    }
}



