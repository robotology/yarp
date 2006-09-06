// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>
#include "FakeFrameGrabber.h"
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main(int argc, char *argv[]) {
    Network::init();

    // give YARP a factory for creating instances of FakeFrameGrabber
    DriverCreator *fakey_factory = 
        new DriverCreatorOf<FakeFrameGrabber>("fakey",
                                              "grabber",
                                              "FakeFrameGrabber");
    Drivers::factory().add(fakey_factory); // hand factory over to YARP

    // use YARP to create and configure an instance of FakeFrameGrabber
    Property config;
    if (argc==1) {
        // no arguments, use a default
        config.fromString("(device fakey) (w 640) (h 480)");
    } else {
        // expect something like "--device fakey --w 640 --h 480"
        //                    or "--device dragonfly"
        //                    or "--device test_grabber --period 0.5 --mode [ball]"
        config.fromCommand(argc,argv);
    }
    PolyDriver dd(config);
    if (!dd.isValid()) {
        printf("Failed to create and configure a device\n");
        exit(1);
    }
    IFrameGrabberImage *grabberInterface;
    if (!dd.view(grabberInterface)) {
        printf("Failed to view device through IFrameGrabberImage interface\n");
        exit(1);
    }

    ImageOf<PixelRgb> img;
    grabberInterface->getImage(img);
    printf("Got a %dx%d image\n", img.width(), img.height());

    dd.close();

    Network::fini();
    return 0;
}
