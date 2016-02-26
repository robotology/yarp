/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>

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


