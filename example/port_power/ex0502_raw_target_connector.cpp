/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

int main() {
    Network yarp;

    Network::connect("/target/raw/out","/target/raw/in"); // connect ports.
    // can do the same thing from command line with 
    // "yarp connect /target/raw/out /target/raw/in"
    
    return 0;
}
