// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Property.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

#include <stdio.h>
#include <stdlib.h>

using namespace yarp::os;
using namespace yarp::dev;

int main(int argc, char *argv[]) {

    // get command line options
    Property options;
    options.fromCommand(argc,argv);

    // check if we're being asked to read the options from file
    BottleBit *val;
    if (options.check("file",val)) {
        ConstString fname = val->toString();
        printf("Working with config file %s\n", fname.c_str());
        options.fromConfigFile(fname);
    }

    // create a device
    PolyDriver dd(options);
    if (!dd.isValid()) {
        printf("Device not available\n");
        exit(1);
    }

    while (1) {
        printf("Device active...\n");
        Time::delay(1);
    }

    return 0;
}

