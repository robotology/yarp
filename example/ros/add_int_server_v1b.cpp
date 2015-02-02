// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <stdlib.h>
#include <yarp/os/all.h>
#include "package/src/yarp_test/srv/AddTwoInts.h"
#include "package/src/yarp_test/srv/AddTwoIntsReply.h"

using namespace yarp::os;

int main(int argc, char *argv[]) {
    Network yarp;
    RpcServer server;

    yarp_test::AddTwoInts example;
    server.promiseType(example.getType());

    if (!server.open("/add_two_ints@/yarp_add_int_server")) {
        fprintf(stderr,"Failed to open port\n");
        return 1;
    }

    while (true) {
        yarp_test::AddTwoInts msg;
        yarp_test::AddTwoIntsReply reply;
        if (!server.read(msg,true)) continue;
        reply.sum = msg.a + msg.b;
        printf("Got %d + %d, answering %d\n", (int)msg.a, (int)msg.b, (int)reply.sum);
        server.reply(reply);
    }

    return 0;
}
