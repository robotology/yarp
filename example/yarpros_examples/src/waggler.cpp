/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <ros/ros.h>
#include <yarpros_examples/VocabVocabDoubles.h>
#include <cstdio>
#include <cstdint>

// A few YARP defines
#define BOTTLE_TAG_INT32 1
#define BOTTLE_TAG_VOCAB32 (1+8)
#define BOTTLE_TAG_FLOAT64 (2+8)
#define BOTTLE_TAG_LIST 256
namespace yarp {
namespace conf {
using vocab32_t = std::int32_t;
} // namespace conf
namespace os {
constexpr yarp::conf::vocab32_t createVocab32(char a, char b = 0, char c = 0, char d = 0)
{
    return (static_cast<yarp::conf::vocab32_t>(a))       +
           (static_cast<yarp::conf::vocab32_t>(b) << 8)  +
           (static_cast<yarp::conf::vocab32_t>(c) << 16) +
           (static_cast<yarp::conf::vocab32_t>(d) << 24);
}
} // namespace os
} // namespace yarp
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
    msg.vocab1_val = yarp::os::createVocab32('s','e','t',0);
    msg.vocab2_tag = BOTTLE_TAG_VOCAB32;
    msg.vocab2_val = yarp::os::createVocab32('p','o','s','s');
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
