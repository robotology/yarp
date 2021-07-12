/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Type.h>

using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::RpcServer;
using yarp::os::Type;

namespace {
YARP_LOG_COMPONENT(SERVER_V1, "yarp.example.ros.add_int_server_v1")
}

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp;
    RpcServer server;

    server.promiseType(Type::byNameOnWire("rospy_tutorials/AddTwoInts"));

    if (!server.open("/add_two_ints@/yarp_add_int_server")) {
        yCError(SERVER_V1, "Failed to open port");
        return 1;
    }

    while (true) {
        Bottle msg;
        Bottle reply;
        if (!server.read(msg, true)) {
            continue;
        }
        int x = msg.get(0).asInt32();
        int y = msg.get(1).asInt32();
        int sum = x + y;
        reply.addInt32(sum);
        yCInfo(SERVER_V1, "Got %d + %d, answering %d", x, y, sum);
        server.reply(reply);
    }

    return 0;
}
