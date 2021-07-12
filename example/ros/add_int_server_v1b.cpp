/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>

#include <yarp/rosmsg/yarp_test/AddTwoInts.h>
#include <yarp/rosmsg/yarp_test/AddTwoIntsReply.h>

using yarp::os::Network;
using yarp::os::RpcServer;

namespace {
YARP_LOG_COMPONENT(SERVER_V1B, "yarp.example.ros.add_int_server_v1b")
}

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp;
    RpcServer server;

    yarp::rosmsg::yarp_test::AddTwoInts example;
    server.promiseType(example.getType());

    if (!server.open("/add_two_ints@/yarp_add_int_server")) {
        yCError(SERVER_V1B, "Failed to open port");
        return 1;
    }

    while (true) {
        yarp::rosmsg::yarp_test::AddTwoInts msg;
        yarp::rosmsg::yarp_test::AddTwoIntsReply reply;
        if (!server.read(msg, true)) {
            continue;
        }
        reply.sum = msg.a + msg.b;
        yCInfo(SERVER_V1B, "Got %ld + %ld, answering %ld", msg.a, msg.b, reply.sum);
        server.reply(reply);
    }

    return 0;
}
