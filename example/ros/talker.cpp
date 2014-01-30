// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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
    Port port;
    port.setWriteOnly();
    if (!port.open("/chatter@/yarp/talker")) {
        fprintf(stderr,"Failed to open port\n");
        return 1;
    }

    for (int i=0; i<1000; i++) {
        char buf[256];
        sprintf(buf,"hello ros %d", i);
        Bottle msg;
        msg.addString(buf);
        port.write(msg);
        printf("Wrote: [%s]\n", buf);
        Time::delay(1);
    }

    return 0;
}
