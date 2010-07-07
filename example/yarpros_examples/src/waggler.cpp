#include <ros/ros.h>
#include <yarpros_examples/VVLCommand.h>
#include <stdio.h>

#define BOTTLE_TAG_INT 1
#define BOTTLE_TAG_VOCAB (1+8)
#define BOTTLE_TAG_DOUBLE (2+8)
#define BOTTLE_TAG_STRING (4)
#define BOTTLE_TAG_BLOB (4+8)
#define BOTTLE_TAG_LIST 256
#define VOCAB(a,b,c,d) ((((int)(d))<<24)+(((int)(c))<<16)+(((int)(b))<<8)+((int)(a)))

int main(int argc, char** argv) {
  int joint_count = 4;
  if (argc>1) {
    joint_count = atoi(argv[1]);
  }
  printf("Working with %d joints\n", joint_count);

  ros::init(argc, argv, "waggler");
  ros::NodeHandle n;
  ros::Publisher waggler_pub = n.advertise<yarpros_examples::VVLCommand>("pos_cmd", 100);
  ros::Rate loop_rate(1);

  int count = 0;
  double *joints = new double[joint_count];
  if (joints==NULL) { printf("out of memory\n"); return 1; }
  while (ros::ok()) {
    for (int i=0; i<joint_count; i++) {
      joints[i] = 0;
    }
    joints[0] = 5*(count%10);

    yarpros_examples::VVLCommand msg;
    msg.list_tag = BOTTLE_TAG_LIST;
    msg.list_len = 3;
    msg.vocab_set_tag = BOTTLE_TAG_VOCAB;
    msg.vocab_set = VOCAB('s','e','t',0);
    msg.vocab_poss_tag = BOTTLE_TAG_VOCAB;
    msg.vocab_poss = VOCAB('p','o','s','s');
    msg.setpoint_list_tag = BOTTLE_TAG_LIST+BOTTLE_TAG_DOUBLE;
    msg.setpoint_list.resize(joint_count);
    for (int i=0; i<joint_count; i++) {
      msg.setpoint_list[i] = joints[i];
    }
    waggler_pub.publish(msg);
    ROS_INFO("I published a command");
    ros::spinOnce();
    loop_rate.sleep();
    ++count;
  }
  delete[] joints;
  return 0;
}
