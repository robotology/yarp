// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <VfwGrabber.h>

#include <yarp/sig/ImageFile.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main() {
    printf("Hello, round 1\n");
    VfwGrabber grabber;
    Property p;
    bool ok = grabber.open(p);
    if (!ok) {
        printf("CAMERA PROBLEM\n");
        return 1;
    }
    for (int i=0; i<15; i++) {
        ImageOf<PixelRgb> img;
        grabber.getImage(img);
        printf("image size %d %d\n", img.width(), img.height());
        char fname[256];
        sprintf(fname,"img%06d.ppm",i);
        yarp::sig::file::write(img,fname);
    }
  return 0;
}

