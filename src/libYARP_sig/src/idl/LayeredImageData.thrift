/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.sig

struct yFlexImage {
} (
  yarp.name = "yarp::sig::FlexImage"
  yarp.includefile="yarp/sig/Image.h"
)

struct yLayer {
} (
  yarp.name = "yarp::sig::ImageLayer"
  yarp.includefile="yarp/sig/ImageLayer.h"
)

struct LayeredImageData
{
    1: yFlexImage background
    2: list<yLayer> layers
}
(
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)
