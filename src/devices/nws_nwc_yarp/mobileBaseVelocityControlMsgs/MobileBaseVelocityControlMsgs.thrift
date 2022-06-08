/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct return_getLastVelocityCommand
{
  1: bool   retvalue;
  2: double x_vel;
  3: double y_vel;
  4: double theta_vel;
}

service MobileBaseVelocityControlRPC
{
  bool applyVelocityCommandRPC(1:double x_vel, 2:double y_vel, 3:double theta_vel, 4:double timeout) ;
  return_getLastVelocityCommand getLastVelocityCommandRPC();
}
