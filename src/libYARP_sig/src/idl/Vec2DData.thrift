/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.sig

typedef i32 ( yarp.type = "size_t" ) size32

struct Vec2DOfDoubleData
{
    1: double x;
    2: double y;
}
(
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)

struct Vec2DOfIntData
{
    1: i32 x;
    2: i32 y;
}
(
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)

struct Vec2DOfSizetData
{
    1: size32 x;
    2: size32 y;
}
(
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)
