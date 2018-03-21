/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <stdio.h>

using namespace yarp::os;

int main() {
    Network yarp;
    Bottle bot;
    Port input;
    input.open("/receiver");
    // usually, we create connections externally, but just for this example...
    Network::connect("/sender","/receiver");
    input.read(bot);
    printf("Got message: %s\n", bot.toString().c_str());
    input.close();
    return 0;
}
