/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

// source for receiver.cpp
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <stdio.h>

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
