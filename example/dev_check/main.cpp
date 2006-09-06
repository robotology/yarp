// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <stdio.h>
#include <stdlib.h>

#include <yarp/sig/Image.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

using namespace yarp::dev;
using namespace yarp::sig;


int main() {
    PolyDriver dd("dragonfly");

    if (!dd.isValid()) {
        printf("Dragonfly not available\n");
        exit(1);
    }

    printf("*** Dragonfly created\n");
  
    IFrameGrabberImage *grabber;
    dd.view(grabber);
  
    if (grabber!=NULL) {
        printf("*** It can supply images\n");
        ImageOf<PixelRgb> img;
        if (grabber->getImage(img)) {
            printf("*** Got a %dx%d image\n", img.width(), img.height());
        } else {
            printf("*** Failed to actually read an image\n");
        }
    } else {
        printf("*** It can <<<<<NOT>>>>> supply images\n");
    }

    IPidControl *pid;
    dd.view(pid);
  
    if (pid!=NULL) {
        printf("*** It can do PID control\n");
    } else {
        printf("*** It can <<<<<NOT>>>>> do PID control\n");
    }
  
    return 0;
}



