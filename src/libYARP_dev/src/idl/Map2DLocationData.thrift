/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct Map2DLocationData
{
    /** name of the map */
    1: string map_id;
    /** x position of the location [m], expressed in the map reference frame */
    2: double x;
    /** y position of the location [m], expressed in the map reference frame  */
    3: double y;
    /** orientation [deg] in the map reference frame */
    4: double theta;
    /** user defined string*/
    5: string description;
}
(
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
