/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// source for receiver.cpp
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <cstdio>

using namespace yarp::os;

int main() {
    Bottle bot2;
    Port input;
    input.open("/in");
    input.read(bot2);
    printf("Got message: %s\n", bot2.toString().c_str());
    input.close();
    return 0;
}
