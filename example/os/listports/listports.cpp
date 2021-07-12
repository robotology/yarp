/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/Value.h>

#include <iostream>

using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::Property;
using yarp::os::Value;

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp;

    std::string name = Network::getNameServerName();
    Bottle msg;
    Bottle reply;
    msg.addString("bot");
    msg.addString("list");

    Network::write(name, msg, reply);

    for (size_t i = 1; i < reply.size(); i++) {
        Property p;
        p.fromString(reply.get(i).toString());

        std::cout << p.check("name", Value("[none]")).asString() << '\n';
    }

    return 0;
}
