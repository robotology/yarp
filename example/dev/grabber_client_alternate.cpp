/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

    ConstString local = "/client";
    ConstString remote = "/fakey";

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
