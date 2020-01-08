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
    if (argc!=3) {
        fprintf(stderr,"Call as %s X Y\n", argv[0]);
        return 1;
    }

    Network yarp;
    RpcClient client;
    yarp::rosmsg::yarp_test::AddTwoInts example;
    client.promiseType(example.getType());

    if (!client.open("/add_two_ints@/yarp_add_int_client")) {
        fprintf(stderr,"Failed to open port\n");
        return 1;
    }

    yarp::rosmsg::yarp_test::AddTwoInts msg;
    yarp::rosmsg::yarp_test::AddTwoIntsReply reply;
    msg.a = atoi(argv[1]);
    msg.b = atoi(argv[2]);
    if (!client.write(msg,reply)) {
        fprintf(stderr,"Failed to call service\n");
        return 1;
    }
    printf("Got %d\n", (int)reply.sum);

    return 0;
}
