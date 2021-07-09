/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/Time.h>

#include <yarp/sig/Vector.h>

using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::Port;
using yarp::os::PortablePair;
using yarp::sig::Vector;

constexpr double loop_delay = 0.25;

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp;
    Port port;
    PortablePair<Bottle, Vector> pp;
    port.open("/pp");
    pp.head.fromString("this is the bottle part");
    int ct = 1;
    int ct2 = 1;
    while (true) {
        Vector v(3);
        v[0] = ct;
        v[1] = ct2;
        v[2] = ct * ct2;
        pp.body = v;
        port.write(pp);
        printf("Sent output to %s...\n", port.getName().c_str());
        ct++;
        if (ct > 10) {
            ct2 = 1 + (ct2 + 1) % 10;
            ct = 1;
        }
        yarp::os::Time::delay(loop_delay);
    }
    return 0;
}
