/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

//! [simple_sender simple_sender.cpp]
#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>

#include <cstdio>

using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::Port;

constexpr double loop_delay = 1.0;
constexpr size_t top = 100;

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp;
    Port output;
    output.open("/sender");
    for (size_t i = 1; i <= top; i++) {
        // prepare a message
        Bottle bot;
        bot.addString("testing");
        bot.addInt32(i);
        bot.addString("of");
        bot.addInt32(top);
        // send the message
        output.write(bot);
        printf("Sent message: %s\n", bot.toString().c_str());
        // wait a while
        yarp::os::Time::delay(loop_delay);
    }
    output.close();
    return 0;
}
//! [simple_sender simple_sender.cpp]
