/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.math

struct yQuaternion {
} (
    yarp.name = "yarp::math::Quaternion"
    yarp.includefile="yarp/math/Quaternion.h"
)

struct yTranslation {
} (
    yarp.name = "yarp::math::Translation"
    yarp.includefile="yarp/math/Translation.h"
)

struct FrameTransformData
{
    1: string src_frame_id;
    2: string dst_frame_id;
    3: double timestamp = 0;
    4: bool   isStatic  = false;
    5: yTranslation translation;
    6: yQuaternion rotation;
}
(
    yarp.api.include = "yarp/math/api.h"
    yarp.api.keyword = "YARP_math_API"
)
