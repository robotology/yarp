/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

namespace yarp yarp.dev

struct Map2DLocation
{
}
(
  yarp.name = "yarp::dev::Nav2D::Map2DLocation"
  yarp.includefile="yarp/dev/Map2DLocation.h"
)

struct Map2DPathData
{
    1: list<Map2DLocation> waypoints;
}
(
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
