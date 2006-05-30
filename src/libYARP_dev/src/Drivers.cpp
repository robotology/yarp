// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/dev/Drivers.h>
#include <yarp/String.h>
#include <yarp/Logger.h>

#include <ace/OS.h>


using namespace yarp::os;
using namespace yarp::dev;


Drivers Drivers::instance;

Drivers::Drivers() {
    implementation = NULL;
}


Drivers::~Drivers() {
}

yarp::os::ConstString Drivers::toString() {
    return ConstString("no description yet");
}


// defined in PopulateDrivers.cpp:
//   DeviceDriver *Drivers::create(const yarp::os::Property& prop);


