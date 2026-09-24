/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Log.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/LogStream.h>

#include <yarp/sig/LayeredImage.h>
#include <yarp/sig/ImageUtils.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <math.h>

using namespace yarp::sig;
using namespace yarp::os;

LayeredImage::LayeredImage()
{
}


LayeredImage::~LayeredImage()
{
}


void LayeredImage::clear()
{
    background.zero();
    layers.clear();
}

LayeredImage::LayeredImage(const LayeredImage& alt) :
        LayeredImageData()
{
    background = alt.background;
    this->layers = alt.layers;
}

LayeredImage::LayeredImage(LayeredImage&& other) noexcept
{
}

LayeredImage& LayeredImage::operator=(const LayeredImage& alt)
{
    background = alt.background;
    this->layers = alt.layers;
    return *this;
}

bool LayeredImage::operator==(const LayeredImage& alt) const
{
    size_t l1 = this->layers.size();
    size_t l2 = alt.layers.size();

    if (l1 != l2)
    {
        return false;
    }

    if (background != alt.background)
    {
        return false;
    }

    for (size_t i = 0; i < l1; i++)
    {
        if ((this->layers[i].enable != alt.layers[i].enable) ||
            (this->layers[i].colorkey.enable != alt.layers[i].colorkey.enable) ||
            (this->layers[i].colorkey.value != alt.layers[i].colorkey.value) ||
            (this->layers[i].alpha.enable != alt.layers[i].alpha.enable) ||
            (fabs(this->layers[i].alpha.value - alt.layers[i].alpha.value) > 0.001) ||
            (this->layers[i].layer != alt.layers[i].layer) ||
            (this->layers[i].can_be_compressed != alt.layers[i].can_be_compressed) ||
            (this->layers[i].offset_x != alt.layers[i].offset_x) ||
            (this->layers[i].offset_y != alt.layers[i].offset_y))
        {
            return false;
        }
    }

    return true;
}

yarp::sig::FlexImage LayeredImage::convert_to_flexImage()
{
    yarp::sig::FlexImage outimg = background;

    bool ret = true;
    for (size_t i = 0; i < this->layers.size(); i++)
    {
        if (layers[i].enable == false)
        {
            continue;
        }

        ret &= yarp::sig::utils::sum(outimg, layers[i].layer, layers[i].colorkey.enable, layers[i].colorkey.value, layers[i].alpha.enable, layers[i].alpha.value, layers[i].offset_x, layers[i].offset_y);
    }

    return outimg;
}

LayeredImage::operator yarp::sig::FlexImage()
{
    return convert_to_flexImage();
}
