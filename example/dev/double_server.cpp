/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
    config.fromString("(device grabber) (subdevice ffmpeg_grabber) (name /dev1/image) (name2 /dev1/sound)");

    PolyDriver dd(config);
    if (!dd.isValid()) {
        printf("Failed to create and configure device 1\n");
        return 1;
    }

    Property config2;
    config2.fromString("(device grabber) (subdevice ffmpeg_grabber) (name /dev2/image) (name2 /dev2/sound)");

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
