/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <yarp/os/all.h>

using namespace yarp::os;

int main(int argc, char *argv[]) {
    Network yarp;
    Node node("/yarp/listener");
    Port port;
    port.setReadOnly();
    if (!port.open("chatter")) {
        fprintf(stderr,"Failed to open port\n");
        return 1;
    }

    while (true) {
        Bottle msg;
        if (!port.read(msg)) {
            fprintf(stderr,"Failed to read msg\n");
            continue;
        }
        printf("Got [%s]\n", msg.get(0).asString().c_str());
    }

    return 0;
}
