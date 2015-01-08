// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/all.h>
#include "std_msgs_String.h"
#include "sensor_msgs_Image.h"

using namespace yarp::os;

int main(int argc, char *argv[]) {
    if (argc<2) return 1;
    bool wait = (argc==3);
    ConstString name = argv[1];
    Node node(name + "/node");
    Publisher<std_msgs_String> topic_string;
    Publisher<sensor_msgs_Image> topic_image;
    if (!topic_string.topic(name + "/str")) return 1;
    if (!topic_image.topic(name + "/img")) return 1;
    while (wait) {
        if (topic_string.asPort().getOutputCount()==0) {
            printf("Waiting for subscriber to string messages\n");
        } else if (topic_image.asPort().getOutputCount()==0) {
            printf("Waiting for subscriber to image messages\n");
        } else {
            printf("Ok\n");
            wait = false;
            continue;
        }
        Time::delay(1);
    }
    for (int i=0; i<5; i++) {
        ConstString txt = "hello world " + Bottle::toString(i);
        topic_string.prepare().data = txt;
        topic_image.prepare();
        printf("Writing %s and empty image\n", txt.c_str());
        topic_string.write();
        topic_image.write();
        Time::delay(0.5);
    }
    return 0;
}
