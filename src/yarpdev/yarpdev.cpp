/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


// The main body of yarpdev is now part of the YARP_dev library, in:
//   src/libYARP_dev/src/yarp/dev/Drivers.cpp

#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/dev/Drivers.h>

int main(int argc, char *argv[]) {
    yarp::os::Network::initMinimum(yarp::os::YARP_CLOCK_SYSTEM); // yarp;
    int ret=yarp::dev::Drivers::yarpdev(argc,argv);
    yarp::os::Network::finiMinimum(); //
    return (ret!=0?1:0);
}
