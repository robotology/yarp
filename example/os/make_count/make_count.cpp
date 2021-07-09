/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <cstdio>

using yarp::os::Bottle;
using yarp::os::BufferedPort;
using yarp::os::Network;

constexpr double loop_delay = 1.0;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return 1;
    }
    Network yarp;

    BufferedPort<Bottle> out;
    out.open(argv[1]);

    for (int i = 10; i >= 0; i--) {
        printf("at %d\n", i);
        Bottle& msg = out.prepare();
        msg.clear();
        msg.addString("countdown");
        msg.addInt32(i);
        out.write();
        yarp::os::Time::delay(loop_delay);
    }
    return 0;
}
