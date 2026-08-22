/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct yBottle {
} (
  yarp.name = "yarp::os::Bottle"
  yarp.includefile = "yarp/os/Bottle.h"
)

struct yVector {
} (
  yarp.name = "yarp::sig::Vector"
  yarp.includefile = "yarp/sig/Vector.h"
)

struct CommandMessageData
{  /** header */
  1: yBottle head;
  /** body */
  2: yVector body;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
