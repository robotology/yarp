/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>

#include <cstdio>

using yarp::os::Bottle;
using yarp::os::BufferedPort;
using yarp::os::Network;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return 1;
    }
    Network yarp;

    BufferedPort<Bottle> in;
    in.open(argv[1]);

    int count = 1;
    while (count > 0) {
        Bottle* msg = in.read();
        count = msg->get(1).asInt32();
        printf("at %d\n", count);
    }

    return 0;
}
