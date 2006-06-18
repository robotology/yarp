// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Property.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

#include <stdio.h>
#include <stdlib.h>

using namespace yarp::os;
using namespace yarp::dev;

int main(int argc, char *argv[]) {

    // just list the devices if no argument given
    if (argc==1) {
        printf("Here are the known devices:\n");
        printf("%s", Drivers::factory().toString().c_str());
        return 0;
    }

    // get command line options
    Property options;
    if (argc==2) {
        // just one value, assume it is a device name
        options.put("device",argv[1]);
    } else {
        // interpret as a set of flags
        options.fromCommand(argc,argv);
    }

    // check if we're being asked to read the options from file
    BottleBit *val;
    if (options.check("file",val)) {
        ConstString fname = val->toString();
        printf("Working with config file %s\n", fname.c_str());
        options.fromConfigFile(fname);
    }

    // check if we want to use lispy nested options (only for Paul's testing)
    if (options.check("lispy",val)) {
        ConstString lispy = val->toString();
        printf("Working with config %s\n", lispy.c_str());
        options.fromString(lispy);
    }

    // create a device
    //printf("Options: %s\n", options.toString().c_str());
    PolyDriver dd(options);
    if (!dd.isValid()) {
        printf("Device not available.  Here are the known devices:\n");
        printf("%s", Drivers::factory().toString().c_str());
        return 1;
    }

    while (dd.isValid()) {
        printf("Device active...\n");
        Time::delay(5);
    }

    return 0;
}

