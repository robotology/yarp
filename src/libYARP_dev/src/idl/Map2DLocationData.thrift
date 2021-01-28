/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

namespace yarp yarp.dev

struct Map2DLocationData
{
    1: string map_id;
    2: double x;
    3: double y;
    4: double theta;
}
(
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
