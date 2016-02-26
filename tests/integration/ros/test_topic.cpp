/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/all.h>
#include "std_msgs_String.h"
#include "sensor_msgs_Image.h"
#include "sensor_msgs_PointCloud2.h"
#include "stereo_msgs_DisparityImage.h"

using namespace yarp::os;

int main(int argc, char *argv[]) {
    if (argc<2) return 1;
    bool wait = (argc==3);
    ConstString name = argv[1];
    Node node(name + "/node");
    Publisher<std_msgs_String> topic_string;
    Publisher<sensor_msgs_Image> topic_image;
    Publisher<stereo_msgs_DisparityImage> topic_disp;
    Publisher<sensor_msgs_PointCloud2> topic_cloud;
    if (!topic_string.topic(name + "/str")) return 1;
    while (wait) {
        if (topic_string.asPort().getOutputCount()>0) break;
        printf("Waiting for subscriber to string messages\n");
        Time::delay(1);
    }
    if (!topic_image.topic(name + "/img")) return 1;
    while (wait) {
        if (topic_image.asPort().getOutputCount()>0) break;
        printf("Waiting for subscriber to image messages\n");
        Time::delay(1);
    }
    if (!topic_disp.topic(name + "/disp")) return 1;
    while (wait) {
        if (topic_disp.asPort().getOutputCount()>0) break;
        printf("Waiting for subscriber to disparity messages\n");
        Time::delay(1);
    }
    if (!topic_cloud.topic(name + "/cloud")) return 1;
    while (wait) {
        if (topic_cloud.asPort().getOutputCount()>0) break;
        printf("Waiting for subscriber to cloud messages\n");
        Time::delay(1);
    }
    for (int i=0; i<5; i++) {
        ConstString txt = "hello world " + Bottle::toString(i);
        topic_string.prepare().data = txt;
        topic_image.prepare();
        topic_disp.prepare();
        topic_cloud.prepare();
        printf("Writing %s and empty image/disparity/cloud\n", txt.c_str());
        topic_string.write();
        Time::delay(0.1);
        topic_image.write();
        Time::delay(0.1);
        topic_disp.write();
        Time::delay(0.1);
        topic_cloud.write();
        Time::delay(0.2);
    }
    return 0;
}
