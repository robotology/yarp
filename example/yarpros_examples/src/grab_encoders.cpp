/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <ros/ros.h>
#include <yarpros_examples/Encoders.h>
#include <stdio.h>

void chatterCallback(const yarpros_examples::EncodersConstPtr& enc)
{
    printf("Encoder readings for first three joints: %g %g %g\n", enc->v[0], enc->v[1], enc->v[2]);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "yarp_encoder_listener");
    ros::NodeHandle n;
    ros::Subscriber chatter_sub = n.subscribe("pos", 1, chatterCallback);
    ros::spin();
}
