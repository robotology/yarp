/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <stdlib.h>

#include <yarp/os/Property.h>

using namespace yarp::os;

int main(int argc, char *argv[]) {
    Property cmdLine;
    cmdLine.fromCommand(argc,argv);

    if (!cmdLine.check("file")) {
        printf("Please call with: --file config.txt\n");
        exit(1);
    }
    ConstString fname = cmdLine.find("file").asString();
    printf("Working with config file %s\n", fname.c_str());

    Property robot;
    robot.fromConfigFile(fname.c_str());
    if (!robot.check("GENERAL")) {
        printf("Cannot understand config file\n");
        exit(1);
    }

    int joints = robot.findGroup("GENERAL").find("Joints").asInt();
    printf("Robot has %d joints\n", joints);

    Bottle& maxes = robot.findGroup("LIMITS").findGroup("Max");
    printf("Robot has limits: ");
    for (int i=1; i<maxes.size(); i++) {
        printf("%d ", maxes.get(i).asInt());
    }
    printf("\n");

    //printf("If you wanted to transmit this configuration, here it is in Bottle format:\n");
    //printf("%s\n", robot.toString().c_str());

    return 0;
}
