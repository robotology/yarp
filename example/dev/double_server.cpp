// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main() {
    Network::init();

    Property config;
    config.fromString("(device grabber) (subdevice ffmpeg_grabber) (name /dev1/image) (name2 /dev1/sound)");

    PolyDriver dd(config);
    if (!dd.isValid()) {
        printf("Failed to create and configure device 1\n");
        exit(1);
    }

    Property config2;
    config2.fromString("(device grabber) (subdevice ffmpeg_grabber) (name /dev2/image) (name2 /dev2/sound)");

    PolyDriver dd2(config2);
    if (!dd2.isValid()) {
        printf("Failed to create and configure device 2\n");
        exit(1);
    }

    while (true) {
        printf("Sending data...");
        Time::delay(5);
    }

    Network::fini();
    return 0;
}
