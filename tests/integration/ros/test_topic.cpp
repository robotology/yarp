/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/all.h>
#include <yarp/rosmsg/std_msgs/String.h>
#include <yarp/rosmsg/sensor_msgs/Image.h>
#include <yarp/rosmsg/sensor_msgs/PointCloud2.h>
#include <yarp/rosmsg/stereo_msgs/DisparityImage.h>

using namespace yarp::os;

int main(int argc, char *argv[]) {
    if (argc<2) return 1;
    bool wait = (argc==3);
    ConstString name = argv[1];
    Node node(name + "/node");
    Publisher<yarp::rosmsg::std_msgs::String> topic_string;
    Publisher<yarp::rosmsg::sensor_msgs::Image> topic_image;
    Publisher<yarp::rosmsg::stereo_msgs::DisparityImage> topic_disp;
    Publisher<yarp::rosmsg::sensor_msgs::PointCloud2> topic_cloud;
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
