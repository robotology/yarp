/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/dev/Drivers.h>

#include "adder.h"

using namespace yarp::os;
using namespace yarp::dev;

int main(int argc, char *argv[]) {
    Network yarp;

    adder();

    int result = Drivers::yarpdev(argc,argv);

    return result;
}
