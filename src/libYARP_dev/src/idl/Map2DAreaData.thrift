/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct Vec2D {
} (
  yarp.name = "yarp::math::Vec2D<double>"
  yarp.includefile="yarp/math/Vec2D.h"
)

struct Map2DAreaData
{
    /** name of the map */
    1: string map_id;
    /** list of points which define the vertices of the area */
    2: list<Vec2D> points;
    /** user defined string*/
    3: string description;
}
(
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
