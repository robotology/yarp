/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.sig

struct ColorRGB
{
    1: i16 r; /* red channel in the range [0-255] */
    2: i16 g; /* green channel in the range [0-255] */
    3: i16 b; /* blue channel in the range [0-255] */
}
(
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)
