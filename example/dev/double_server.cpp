/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main() {
    Network yarp;

    Property config;
    config.fromString("(device grabberDual) (subdevice ffmpeg_grabber) (name /dev1/image) (name2 /dev1/sound)");

    PolyDriver dd(config);
    if (!dd.isValid()) {
        printf("Failed to create and configure device 1\n");
        return 1;
    }

    Property config2;
    config2.fromString("(device grabberDual) (subdevice ffmpeg_grabber) (name /dev2/image) (name2 /dev2/sound)");

    PolyDriver dd2(config2);
    if (!dd2.isValid()) {
        printf("Failed to create and configure device 2\n");
        return 1;
    }

    while (true) {
        printf("Sending data...");
        Time::delay(5);
    }

    return 0;
}
