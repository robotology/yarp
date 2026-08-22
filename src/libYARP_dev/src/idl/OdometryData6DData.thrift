/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct OdometryData6D
{
  /** position of the robot [m], expressed in the world reference frame */
   1: double odom_x;
  /** position of the robot [m], expressed in the world reference frame */
   2: double odom_y;
  /** position of the robot [m], expressed in the world reference frame */
   3: double odom_z;
  /** orientation the robot [deg], expressed in the world reference frame */
   4: double odom_roll;
  /** orientation the robot [deg], expressed in the world reference frame */
   5: double odom_pitch;
  /** orientation the robot [deg], expressed in the world reference frame */
   6: double odom_yaw;

  /** velocity of the robot [m/s] expressed in the robot reference frame */
   7: double base_vel_x;
  /** velocity of the robot [m/s] expressed in the robot reference frame */
   8: double base_vel_y;
  /** velocity of the robot [m/s] expressed in the robot reference frame */
   9: double base_vel_z;
  /** angular velocity of the robot [deg/s] expressed in the robot reference frame */
  10: double base_vel_roll;
  /** angular velocity of the robot [deg/s] expressed in the robot reference frame */
  11: double base_vel_pitch;
  /** angular velocity of the robot [deg/s] expressed in the robot reference frame */
  12: double base_vel_yaw;

  /** velocity of the robot [m/s] expressed in the world reference frame */
  13: double odom_vel_x;
  /** velocity of the robot [m/s] expressed in the world reference frame */
  14: double odom_vel_y;
  /** velocity of the robot [m/s] expressed in the world reference frame */
  15: double odom_vel_z;
  /** angular velocity of the robot [deg/s] expressed in the world reference frame */
  16: double odom_vel_roll;
  /** angular velocity of the robot [deg/s] expressed in the world reference frame */
  17: double odom_vel_pitch;
  /** angular velocity of the robot [deg/s] expressed in the world reference frame */
  18: double odom_vel_yaw;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
