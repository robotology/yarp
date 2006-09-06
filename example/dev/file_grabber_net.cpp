// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>
#include "FileFrameGrabber.h"
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main() {
    Network::init();

    // give YARP a factory for creating instances of FileFrameGrabber
    DriverCreator *file_grabber_factory = 
        new DriverCreatorOf<FileFrameGrabber>("file_grabber",
                                              "grabber",
                                              "FileFrameGrabber");
    Drivers::factory().add(file_grabber_factory); // hand factory over to YARP

    // use YARP to create and configure a networked of FakeFrameGrabber
    Property config;
    // You may have to tweak the "image" path to match where your executable is
    config.fromString("(device grabber) (name /file) (subdevice file_grabber) (pattern \"image/img%04d.ppm\") (first 250)");
    PolyDriver dd(config);
    if (!dd.isValid()) {
        printf("Failed to create and configure a device\n");
        exit(1);
    }

    // snooze while the network device operates
    while (true) {
        printf("Network device is active...\n");
        Time::delay(5);
    }

    Network::fini();
    return 0;
}
