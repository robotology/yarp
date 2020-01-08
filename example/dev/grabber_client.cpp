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

/*
 * Read an image from a remote source using the "device" view of
 * camera-like sources.
 *
 * Remote source could be, for example:
 *   yarpdev --device test_grabber --name /fakey
 *
 */

int main() {
    Network yarp;

    Property config;
    config.put("device","remote_grabber"); // device type
    config.put("local","/client");         // name of local port to use
    config.put("remote","/fakey");         // name of remote port to connect to

    PolyDriver dd(config);
    if (!dd.isValid()) {
        printf("Failed to create and configure device\n");
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
