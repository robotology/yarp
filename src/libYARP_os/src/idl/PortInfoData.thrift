/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.os

struct PortInfoData
{
    1: bool         is_input;
    2: bool         is_output;
    3: bool         is_rpc;
    4: string       type;
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)
