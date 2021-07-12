/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct MobileBaseVelocity
{
  /** velocity of the robot [m/s] */
  4: double vel_x;
  /** velocity of the robot [m/s]*/
  5: double vel_y;
  /** angular velocity of the robot [deg/s] */
  6: double vel_theta;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
