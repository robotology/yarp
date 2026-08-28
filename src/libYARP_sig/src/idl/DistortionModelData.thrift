/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.sig


enum CameraDistortionType{
    YARP_DISTORTION_NONE=0,  // Rectilinear images. No distortion compensation required.
    YARP_PLUMB_BOB=1,        // Plumb bob distortion model
    YARP_FISH_EYE=2,         // Fish eye distortion model
    YARP_UNSUPPORTED=3       // Unsupported distortion model
}
(
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)

struct DistortionModelData
{
    1: double k1;
    2: double k2;
    3: double k3;
    4: double t1;
    5: double t2;
    6: CameraDistortionType type;
}
(
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)
