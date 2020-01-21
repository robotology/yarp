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
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>
#include "FileFrameGrabber.h"
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main() {
    Network yarp;

    // give YARP a factory for creating instances of FileFrameGrabber
    DriverCreator *file_grabber_factory =
        new DriverCreatorOf<FileFrameGrabber>("file_grabber",
                                              "grabberDual",
                                              "FileFrameGrabber");
    Drivers::factory().add(file_grabber_factory); // hand factory over to YARP

    // use YARP to create and configure a networked of FakeFrameGrabber
    Property config;
    // You may have to tweak the "image" path to match where your executable is
    config.fromString("(device grabberDual) (name /file) (subdevice file_grabber) (pattern \"image/img%04d.ppm\") (first 250)");
    PolyDriver dd(config);
    if (!dd.isValid()) {
        printf("Failed to create and configure a device\n");
        return 1;
    }

    // snooze while the network device operates
    while (true) {
        printf("Network device is active...\n");
        Time::delay(5);
    }

    return 0;
}
