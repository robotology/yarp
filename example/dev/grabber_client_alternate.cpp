// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
    Network::init();

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

    Network::fini();
    return 0;
}
