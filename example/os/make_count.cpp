/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/all.h>
#include <stdio.h>
using namespace yarp::os;

int main(int argc, char *argv[]) {
    if (argc!=2) return 1;
    Network yarp;

    BufferedPort<Bottle> out;
    out.open(argv[1]);

    for (int i=10; i>=0; i--) {
        printf("at %d\n", i);
        Bottle& msg = out.prepare();
        msg.clear();
        msg.addString("countdown");
        msg.addInt32(i);
        out.write();
        Time::delay(1);
    }
    return 0;
}
