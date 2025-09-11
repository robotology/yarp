/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct Map2DObjectData
{
    /** name of the map */
    1: string map_id;
    /** x position of the location [m], expressed in the map reference frame */
    2: double x;
    /** y position of the location [m], expressed in the map reference frame  */
    3: double y;
    /** z position of the location [m], expressed in the map reference frame  */
    4: double z;
    /** orientation [deg] in the map reference frame */
    5: double roll;
    /** orientation [deg] in the map reference frame */
    6: double pitch;
    /** orientation [deg] in the map reference frame */
    7: double yaw;
    /** user defined string*/
    8: string description;
}
(
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
