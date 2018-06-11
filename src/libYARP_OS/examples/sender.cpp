/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// source for sender.cpp
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <cstdio>

using namespace yarp::os;

int main() {
    Bottle bot1;
    bot1.addString("testing"); // a simple message
    Port output;
    output.open("/out");
    for (int i=0; i<100; i++) {
        output.write(bot1);
        printf("Sent message: %s\n", bot1.toString().c_str());
        bot1.addInt32(i); // change the message for next time
        Time::delay(1);
    }
    output.close();
    return 0;
}
