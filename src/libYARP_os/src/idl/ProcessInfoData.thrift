/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.os

struct ProcessInfoData
{
    1: i32          pid;
    2: string       name;
    3: string       arguments;
    4: i32          priority;
    5: i32          policy;
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)
