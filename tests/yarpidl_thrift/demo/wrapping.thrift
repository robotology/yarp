/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct WrapValue
{
}
(
  yarp.name = "yarp::os::Value"
  yarp.includefile="yarp/os/Value.h"
)

struct WrapBottle
{
}
(
  yarp.name = "yarp::os::Bottle"
  yarp.includefile="yarp/os/Bottle.h"
)

service Wrapping {
  i32 check(1:WrapValue param);
  WrapBottle getBottle();
}
