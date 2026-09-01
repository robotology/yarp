/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.os

struct ConnectionQosData
{
    1: string      portname;
    2: i32         scheduler_priority   
    3: i32         scheduler_policy
    4: i32         qos_tos
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)
