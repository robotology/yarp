/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <ros/ros.h>
#include <yarpros_examples/VocabVocabDoubles.h>
#include <stdio.h>

// A few YARP defines
#define BOTTLE_TAG_INT32 1
#define BOTTLE_TAG_VOCAB32 (1+8)
#define BOTTLE_TAG_FLOAT64 (2+8)
#define BOTTLE_TAG_LIST 256
#define yarp::os::createVocab(a,b,c,d) ((((int)(d))<<24)+(((int)(c))<<16)+(((int)(b))<<8)+((int)(a)))
// YARP defines over

int main(int argc, char** argv) {
  int joint_count = 16;
  if (argc>1) {
    joint_count = atoi(argv[1]);
  }
  printf("Working with %d joints\n", joint_count);

  ros::init(argc, argv, "waggler");
  ros::NodeHandle n;
  ros::Publisher waggler_pub = n.advertise<yarpros_examples::VocabVocabDoubles>("motor", 100);
  ros::Rate loop_rate(1.0);

  int count = 0;
  while (ros::ok()) {

    yarpros_examples::VocabVocabDoubles msg;
    msg.list_tag = BOTTLE_TAG_LIST;
    msg.list_len = 3;
    msg.vocab1_tag = BOTTLE_TAG_VOCAB32;
    msg.vocab1_val = yarp::os::createVocab('s','e','t',0);
    msg.vocab2_tag = BOTTLE_TAG_VOCAB32;
    msg.vocab2_val = yarp::os::createVocab('p','o','s','s');
    msg.setpoints_tag = BOTTLE_TAG_LIST+BOTTLE_TAG_FLOAT64;
    msg.setpoints.resize(joint_count);
    for (int i=0; i<joint_count; i++) {
      msg.setpoints[i] = 0;
    }
    msg.setpoints[0] = -50*(count%2);
    msg.setpoints[1] = 30+20*(count%2);
    msg.setpoints[3] = 25*(count%4);

    waggler_pub.publish(msg);
    ROS_INFO("I published a command");
    ros::spinOnce();
    loop_rate.sleep();
    ++count;
  }
  return 0;
}
