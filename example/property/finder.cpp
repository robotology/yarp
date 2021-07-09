/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include <yarp/os/ResourceFinder.h>

using namespace yarp::os;

int main(int argc, char *argv[]) {
    ResourceFinder robot;
    robot.configure(argc,argv);

    if (!robot.check("GENERAL")) {
        printf("Cannot find needed configuration (try --from config.txt)\n");
        return 1;
    }

    int joints = robot.findGroup("GENERAL").find("Joints").asInt32();
    printf("Robot has %d joints\n", joints);

    Bottle& maxes = robot.findGroup("LIMITS").findGroup("Max");
    printf("Robot has limits: ");
    for (int i=1; i<maxes.size(); i++) {
        printf("%d ", maxes.get(i).asInt32());
    }
    printf("\n");

    return 0;
}
