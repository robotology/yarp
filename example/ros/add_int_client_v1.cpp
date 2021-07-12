/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/Network.h>
#include <yarp/os/RpcClient.h>

using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::RpcClient;

namespace {
YARP_LOG_COMPONENT(CLIENT_V1, "yarp.example.ros.add_int_client_v1")
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        yCError(CLIENT_V1, "Call as %s X Y", argv[0]);
        return 1;
    }

    Network yarp;
    RpcClient client;
    if (!client.open("/add_two_ints@/yarp_add_int_client")) {
        yCError(CLIENT_V1, "Failed to open port");
        return 1;
    }

    Bottle msg;
    Bottle reply;
    msg.addInt32(atoi(argv[1]));
    msg.addInt32(atoi(argv[2]));
    if (!client.write(msg, reply)) {
        yCError(CLIENT_V1, "Failed to call service");
        return 1;
    }
    yCInfo(CLIENT_V1, "Got %d", reply.get(0).asInt32());

    return 0;
}
