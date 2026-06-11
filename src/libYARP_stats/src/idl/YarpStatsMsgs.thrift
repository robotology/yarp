/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.stats

struct ConnectionStats {
    1: double timestamp;
    2: string source;
    3: string destination;
    4: string carrier;
    5: bool   isForward;
    6: double bytes_per_second;
    7: double frequency;
}
(
    yarp.api.include = "yarp/stats/api.h"
    yarp.api.keyword = "YARP_stats_API"
)
