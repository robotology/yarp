/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_IMAGE_LAYER_H
#define YARP_IMAGE_LAYER_H

#include <yarp/sig/ImageLayerData.h>

namespace yarp::sig {
class ImageLayer;
} // namespace yarp::sig

class yarp::sig::ImageLayer : public yarp::sig::ImageLayerData
{
    public:
    ImageLayer() = default;
    ImageLayer(const yarp::sig::FlexImage& img, bool ena = true, yarp::sig::ImageColorKey ckey = yarp::sig::ImageColorKey(), yarp::sig::ImageAlphaData alph = yarp::sig::ImageAlphaData(), bool compress = true, int off_x = 0, int off_y = 0)
    {
        layer = img;
        enable = ena;
        colorkey = ckey;
        alpha = alph;
        can_be_compressed = compress;
        offset_x = off_x;
        offset_y = off_y;
    }
};

#endif // YARP_LAYERED_IMAGE_H
