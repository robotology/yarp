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

int main(int argc, char *argv[]) {
    Network yarp;

    // give YARP a factory for creating instances of FileFrameGrabber
    DriverCreator *file_grabber_factory =
        new DriverCreatorOf<FileFrameGrabber>("file_grabber",
                                              "grabber",
                                              "FileFrameGrabber");
    Drivers::factory().add(file_grabber_factory); // hand factory over to YARP

    // use YARP to create and configure an instance of FileFrameGrabber
    Property config;
    if (argc==1) {
        // no arguments, use a default
        config.fromString("(device file_grabber) (pattern \"image/%03d.ppm\")");
    } else {
        // expect something like '--device file_grabber --pattern "image/%03d.ppm"'
        //                    or '--device dragonfly'
        //                    or '--device test_grabber --period 0.5 --mode [ball]'
        config.fromCommand(argc,argv);
    }
    PolyDriver dd(config);
    if (!dd.isValid()) {
        printf("Failed to create and configure a device\n");
        return 1;
    }
    IFrameGrabberImage *grabberInterface;
    if (!dd.view(grabberInterface)) {
        printf("Failed to view device through IFrameGrabberImage interface\n");
        return 1;
    }

    ImageOf<PixelRgb> img;
    grabberInterface->getImage(img);
    printf("Got a %dx%d image\n", img.width(), img.height());

    dd.close();

    return 0;
}
