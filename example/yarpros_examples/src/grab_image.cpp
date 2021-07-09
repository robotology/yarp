/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
