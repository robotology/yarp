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

using namespace yarp::os;

int main(int argc, char *argv[]) {
    Network yarp;
    RpcServer server;

    server.promiseType(Type::byNameOnWire("rospy_tutorials/AddTwoInts"));

    if (!server.open("/add_two_ints@/yarp_add_int_server")) {
        fprintf(stderr,"Failed to open port\n");
        return 1;
    }

    while (true) {
        Bottle msg, reply;
        if (!server.read(msg,true)) continue;
        int x = msg.get(0).asInt32();
        int y = msg.get(1).asInt32();
        int sum = x + y;
        reply.addInt32(sum);
        printf("Got %d + %d, answering %d\n", x, y, sum);
        server.reply(reply);
    }

    return 0;
}
