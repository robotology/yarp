/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Image.h>

#include "FileFrameGrabber.h"

#include <cstdio>

using yarp::os::Network;
using yarp::os::Property;
using yarp::dev::DriverCreator;
using yarp::dev::DriverCreatorOf;
using yarp::dev::Drivers;
using yarp::dev::PolyDriver;
using yarp::dev::IFrameGrabberImage;
using yarp::sig::ImageOf;
using yarp::sig::PixelRgb;

int main(int argc, char* argv[])
{
    Network yarp;

    // give YARP a factory for creating instances of FileFrameGrabber
    DriverCreator* file_grabber_factory = new DriverCreatorOf<FileFrameGrabber>("file_grabber",
                                                                                "grabber",
                                                                                "FileFrameGrabber");
    Drivers::factory().add(file_grabber_factory); // hand factory over to YARP

    // use YARP to create and configure an instance of FileFrameGrabber
    Property config;
    if (argc == 1) {
        // no arguments, use a default
        config.fromString("(device file_grabber) (pattern \"image/%03d.ppm\")");
    } else {
        // expect something like '--device file_grabber --pattern "image/%03d.ppm"'
        //                    or '--device dragonfly'
        //                    or '--device fakeFrameGrabber --period 0.5 --mode [ball]'
        config.fromCommand(argc, argv);
    }
    PolyDriver dd(config);
    if (!dd.isValid()) {
        printf("Failed to create and configure a device\n");
        return 1;
    }
    IFrameGrabberImage* grabberInterface;
    if (!dd.view(grabberInterface)) {
        printf("Failed to view device through IFrameGrabberImage interface\n");
        return 1;
    }

    ImageOf<PixelRgb> img;
    grabberInterface->getImage(img);
    printf("Got a %zux%zu image\n", img.width(), img.height());

    dd.close();

    return 0;
}
