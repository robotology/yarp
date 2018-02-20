/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>

#include <yarp/os/all.h>
using namespace yarp::os;

int main(int argc, char *argv[]) {
    int result = 0;

    printf("This test creates a port and then immediately closes it.\n");
    printf("Make sure no other YARP programs are running.\n");
    printf("(or else remove the yarpNetworkSetLocalMode line)\n");

    Network yarp;
    yarp.setLocalMode(true);

    Port port;
    bool ok = port.open("/test");
    if (!ok) { fprintf(stderr, "failed to open port /test\n"); }

    return 0;
}


