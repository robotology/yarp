/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
