/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>

using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::Port;

namespace {
YARP_LOG_COMPONENT(LISTENER_V1, "yarp.example.ros.listener_v1")
}

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp;
    Port port;
    port.setReadOnly();
    if (!port.open("/chatter@/yarp/listener")) {
        yCError(LISTENER_V1, "Failed to open port");
        return 1;
    }

    while (true) {
        Bottle msg;
        if (!port.read(msg)) {
            yCError(LISTENER_V1, "Failed to read msg");
            continue;
        }
        yCInfo(LISTENER_V1, "Got [%s]", msg.get(0).asString().c_str());
    }

    return 0;
}
