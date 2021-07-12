/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
        out.addString("hello");
        out.addString("world");
        out.addInt32(ct);
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
