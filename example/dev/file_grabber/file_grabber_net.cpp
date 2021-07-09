/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FileFrameGrabber.h"

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/Time.h>

#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

#include <cstdio>

using yarp::dev::DriverCreator;
using yarp::dev::DriverCreatorOf;
using yarp::dev::Drivers;
using yarp::dev::PolyDriver;
using yarp::os::Network;
using yarp::os::Property;

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp;

    // give YARP a factory for creating instances of FileFrameGrabber
    DriverCreator* file_grabber_factory = new DriverCreatorOf<FileFrameGrabber>("file_grabber",
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
        yarp::os::Time::delay(5);
    }

    return 0;
}
