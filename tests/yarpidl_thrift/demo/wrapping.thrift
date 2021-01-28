/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
