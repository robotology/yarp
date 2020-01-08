/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <yarp/os/all.h>
#include <yarp/rosmsg/yarp_test/AddTwoInts.h>
#include <yarp/rosmsg/yarp_test/AddTwoIntsReply.h>

using namespace yarp::os;

int main(int argc, char *argv[]) {
    Network yarp;
    RpcServer server;

    yarp::rosmsg::yarp_test::AddTwoInts example;
    server.promiseType(example.getType());

    if (!server.open("/add_two_ints@/yarp_add_int_server")) {
        fprintf(stderr,"Failed to open port\n");
        return 1;
    }

    while (true) {
        yarp::rosmsg::yarp_test::AddTwoInts msg;
        yarp::rosmsg::yarp_test::AddTwoIntsReply reply;
        if (!server.read(msg,true)) continue;
        reply.sum = msg.a + msg.b;
        printf("Got %d + %d, answering %d\n", (int)msg.a, (int)msg.b, (int)reply.sum);
        server.reply(reply);
    }

    return 0;
}
