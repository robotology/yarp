// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


// The main body of yarpdev is now part of the libYARP_dev library, in:
//   src/libYARP_dev/src/Drivers.cpp
// -paulfitz

#include <yarp/os/Network.h>
#include <yarp/dev/Drivers.h>

#include "yarpdev_drivers.h"

int main(int argc, char *argv[]) {
    yarp::dev::DriverCollection dev;
	yarp::os::Network yarp;
    return yarp::dev::Drivers::yarpdev(argc,argv);
}

