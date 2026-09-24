/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.sig

struct SoundMarkerData
{
    1: string label;        /* marker name */
    2: i32 channel = -1;    /* on channel number */
    3: i32 sample_id = 0;   /* at sample number */
}
(
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)
