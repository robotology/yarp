/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.sig

struct yDistortionModel {
} (
  yarp.name = "yarp::sig::DistortionModelData"
  yarp.includefile="yarp/sig/DistortionModelData.h"
)

struct yVector {
} (
  yarp.name = "yarp::sig::Vector"
  yarp.includefile = "yarp/sig/Vector.h"
)

struct IntrinsicParamsData
{
    1: double   physFocalLength;         // Physical focal length of the lens (m)
    2: double   principalPointX;         // Horizontal coordinate of the principal point of the image, as a pixel offset from the left edge
    3: double   principalPointY;         // Vertical coordinate of the principal point of the image, as a pixel offset from the top edge
    4: double   focalLengthX;            // Result of the product of the physical focal length(mm) and the size sx of the individual image elements (pixels per mm)
    5: double   focalLengthY;            // Result of the product of the physical focal length(mm) and the size sy of the individual image elements (pixels per mm)
    6: yDistortionModel distortionModel; // Distortion model of the image
    7: yVector  rectificationMatrix3X3;  // Represented as a vector of 9 elements
    8: bool     isOptional;
}
(
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)
