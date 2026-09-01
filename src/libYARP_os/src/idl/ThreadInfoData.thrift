/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.os

struct ThreadInfoData
{
    1: i32          tid;
    4: i32          priority;
    5: i32          policy;
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)
