/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.sig

struct yColorKey {
} (
  yarp.name = "yarp::sig::ImageColorKey"
  yarp.includefile="yarp/sig/ImageColorKey.h"
)

struct yAlpha {
} (
  yarp.name = "yarp::sig::ImageAlphaData"
  yarp.includefile="yarp/sig/ImageAlphaData.h"
)

struct yFlexImage {
} (
  yarp.name = "yarp::sig::FlexImage"
  yarp.includefile="yarp/sig/Image.h"
)

/**
 * \ingroup sig_class
 *
 * A single layer of a layered image
 */
struct ImageLayerData
{
    1: bool        enable = 1
    2: yFlexImage  layer
    3: yColorKey   colorkey
    4: yAlpha      alpha
    5: bool        can_be_compressed = 1
    6: i32         offset_x = 0
    7: i32         offset_y = 0
}
(
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)
