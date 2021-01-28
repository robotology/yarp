/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <ros/ros.h>
#include <yarpros_examples/YarpImageRgb.h>
#include <stdio.h>

void chatterCallback(const yarpros_examples::YarpImageRgbConstPtr& img)
{
    printf("Got image of size %dx%d\n", img->v2[3], img->v2[4]);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "yarp_image_listener");
    ros::NodeHandle n;
    ros::Subscriber chatter_sub = n.subscribe("yarp_image", 100, chatterCallback);
    ros::spin();
}
