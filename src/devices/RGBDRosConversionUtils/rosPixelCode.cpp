/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "rosPixelCode.h"

namespace yarp {
    namespace dev {
        namespace ROSPixelCode {

std::string yarp2RosPixelCode(int code)
{
    switch (code)
    {
    case VOCAB_PIXEL_BGR:
        return BGR8;
    case VOCAB_PIXEL_BGRA:
        return BGRA8;
    case VOCAB_PIXEL_ENCODING_BAYER_BGGR16:
        return BAYER_BGGR16;
    case VOCAB_PIXEL_ENCODING_BAYER_BGGR8:
        return BAYER_BGGR8;
    case VOCAB_PIXEL_ENCODING_BAYER_GBRG16:
        return BAYER_GBRG16;
    case VOCAB_PIXEL_ENCODING_BAYER_GBRG8:
        return BAYER_GBRG8;
    case VOCAB_PIXEL_ENCODING_BAYER_GRBG16:
        return BAYER_GRBG16;
    case VOCAB_PIXEL_ENCODING_BAYER_GRBG8:
        return BAYER_GRBG8;
    case VOCAB_PIXEL_ENCODING_BAYER_RGGB16:
        return BAYER_RGGB16;
    case VOCAB_PIXEL_ENCODING_BAYER_RGGB8:
        return BAYER_RGGB8;
    case VOCAB_PIXEL_MONO:
        return MONO8;
    case VOCAB_PIXEL_MONO16:
        return MONO16;
    case VOCAB_PIXEL_RGB:
        return RGB8;
    case VOCAB_PIXEL_RGBA:
        return RGBA8;
    case VOCAB_PIXEL_MONO_FLOAT:
        return TYPE_32FC1;
    default:
        return RGB8;
    }
}

int Ros2YarpPixelCode(const std::string& roscode)
{
    if (roscode == BGR8)
        return VOCAB_PIXEL_BGR;
    else if (roscode == BGRA8)
        return VOCAB_PIXEL_BGRA;
    else if (roscode == BAYER_BGGR8)
        return VOCAB_PIXEL_ENCODING_BAYER_BGGR8;
    else if (roscode == BAYER_GBRG16)
        return VOCAB_PIXEL_ENCODING_BAYER_GBRG16;
    else if (roscode == BAYER_GBRG8)
        return VOCAB_PIXEL_ENCODING_BAYER_GBRG8;
    else if (roscode == BAYER_GRBG16)
        return VOCAB_PIXEL_ENCODING_BAYER_GRBG16;
    else if (roscode == BAYER_GRBG8)
        return VOCAB_PIXEL_ENCODING_BAYER_GRBG8;
    else if (roscode == BAYER_RGGB16)
        return VOCAB_PIXEL_ENCODING_BAYER_RGGB16;
    else if (roscode == BAYER_RGGB8)
        return VOCAB_PIXEL_ENCODING_BAYER_RGGB8;
    else if (roscode == MONO8)
        return VOCAB_PIXEL_MONO;
    else if (roscode == MONO16)
        return VOCAB_PIXEL_MONO16;
    else if (roscode == RGB8)
        return VOCAB_PIXEL_RGB;
    else if (roscode == RGBA8)
        return VOCAB_PIXEL_RGBA;
    else if (roscode == TYPE_32FC1)
        return VOCAB_PIXEL_MONO_FLOAT;
    else
        return VOCAB_PIXEL_INVALID;
}

}}}
