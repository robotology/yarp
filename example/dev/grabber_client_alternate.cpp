/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace yarp::os;
using namespace yarp::sig;

/*
 * Read an image from a remote source using the "port" view of
 * camera-like sources.
 *
 * Remote source could be, for example:
 *   yarpdev --device test_grabber --name /fakey
 *
 */

int main() {
    Network yarp;

    std::string local = "/client";
    std::string remote = "/fakey";

    BufferedPort<ImageOf<PixelRgb> > port;
    port.open(local);
    Network::connect(remote,local);
    ImageOf<PixelRgb> *img = port.read();
    if (img!=NULL) {
        printf("Got a %dx%d image\n", img->width(), img->height());
    }
    port.close();

    return 0;
}
