/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>

#include <yarp/os/Time.h>
#include <yarp/os/Network.h>

#include <iostream>

using namespace yarp::os;

int main() {
    printf("Hello...\n");
    Time::delay(1);
    printf("...world\n");

   //Yarp network initialization
    yarp::os::Network yarp;
    if (!yarp.checkNetwork()) {
        std:: cerr << "Cannot connect to yarp network";
    }

    return 0;
}
