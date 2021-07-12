/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct OdometryData
{
  /** position of the robot [m], expressed in the world reference frame */
  1: double odom_x;
  /** position of the robot [m], expressed in the world reference frame */
  2: double odom_y;
  /** orientation the robot [deg], expressed in the world reference frame */
  3: double odom_theta;

  /** velocity of the robot [m/s] expressed in the robot reference frame */
  4: double base_vel_x;
  /** velocity of the robot [m/s] expressed in the robot reference frame */
  5: double base_vel_y;
  /** angular velocity of the robot [deg/s] expressed in the robot reference frame */
  6: double base_vel_theta;

  /** velocity of the robot [m/s] expressed in the world reference frame */
  7: double odom_vel_x;
  /** velocity of the robot [m/s] expressed in the world reference frame */
  8: double odom_vel_y;
  /** angular velocity of the robot [deg/s] expressed in the world reference frame */
  9: double odom_vel_theta;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
