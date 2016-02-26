/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
