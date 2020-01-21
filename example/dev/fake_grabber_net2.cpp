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
#include "FakeFrameGrabber2.h"
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main() {
    Network yarp;

    // give YARP a factory for creating instances of FakeFrameGrabber
    DriverCreator *fakey_factory =
        new DriverCreatorOf<FakeFrameGrabber>("fakey",
                                              "grabberDual",
                                              "FakeFrameGrabber");
    Drivers::factory().add(fakey_factory); // hand factory over to YARP

    // use YARP to create and configure a networked of FakeFrameGrabber
    Property config;
    config.fromString("(device grabberDual) (name /fakey) (subdevice fakey) (w 200) (h 200)");
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
