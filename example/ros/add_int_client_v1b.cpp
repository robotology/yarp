/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/Network.h>
#include <yarp/os/RpcClient.h>

#include <yarp/rosmsg/yarp_test/AddTwoInts.h>
#include <yarp/rosmsg/yarp_test/AddTwoIntsReply.h>

using yarp::os::Network;
using yarp::os::RpcClient;

namespace {
YARP_LOG_COMPONENT(CLIENT_V1B, "yarp.example.ros.add_int_client_v1b")
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        yCError(CLIENT_V1B, "Call as %s X Y", argv[0]);
        return 1;
    }

    Network yarp;
    RpcClient client;
    yarp::rosmsg::yarp_test::AddTwoInts example;
    client.promiseType(example.getType());

    if (!client.open("/add_two_ints@/yarp_add_int_client")) {
        yCError(CLIENT_V1B, "Failed to open port");
        return 1;
    }

    yarp::rosmsg::yarp_test::AddTwoInts msg;
    yarp::rosmsg::yarp_test::AddTwoIntsReply reply;
    msg.a = atoi(argv[1]);
    msg.b = atoi(argv[2]);
    if (!client.write(msg, reply)) {
        yCError(CLIENT_V1B, "Failed to call service");
        return 1;
    }
    yCInfo(CLIENT_V1B, "Got %ld\n", reply.sum);

    return 0;
}
