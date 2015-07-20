// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
