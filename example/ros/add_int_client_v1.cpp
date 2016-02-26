/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <stdlib.h>
#include <yarp/os/all.h>

using namespace yarp::os;

int main(int argc, char *argv[]) {
    if (argc!=3) {
        fprintf(stderr,"Call as %s X Y\n", argv[0]);
        return 1;
    }

    Network yarp;
    RpcClient client;
    if (!client.open("/add_two_ints@/yarp_add_int_client")) {
        fprintf(stderr,"Failed to open port\n");
        return 1;
    }

    Bottle msg, reply;
    msg.addInt(atoi(argv[1]));
    msg.addInt(atoi(argv[2]));
    if (!client.write(msg,reply)) {
        fprintf(stderr,"Failed to call service\n");
        return 1;
    }
    printf("Got %d\n", reply.get(0).asInt());

    return 0;
}
