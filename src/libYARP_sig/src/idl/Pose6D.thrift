/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.sig

struct Pose6D
{
    1: double x;     /* x position [m] */
    2: double y;     /* y position [m] */
    3: double z;     /* z position [m] */
    4: double roll;  /* rotation along roll axis, [rad] */
    5: double pitch; /* rotation along pitch axis [rad]*/
    6: double yaw;   /* rotation along yaw axis [rad]*/
}
(
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)
