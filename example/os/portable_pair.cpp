/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char *argv[]) {
    Network yarp;
    Port port;
    PortablePair<Bottle,Vector> pp;
    port.open("/pp");
    pp.head.fromString("this is the bottle part");
    int ct = 1;
    int ct2 = 1;
    while (true) {
        Vector v(3);
        v[0] = ct;
        v[1] = ct2;
        v[2] = ct*ct2;
        pp.body = v;
        port.write(pp);
        printf("Sent output to %s...\n", port.getName().c_str());
        ct++;
        if (ct>10) {
            ct2 = 1+(ct2+1)%10;
            ct = 1;
        }
        Time::delay(0.25);
    }
    return 0;
}
