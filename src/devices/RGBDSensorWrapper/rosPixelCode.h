/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_RGBDSENSORWRAPPER_ROSPIXELCODE_H
#define YARP_DEV_RGBDSENSORWRAPPER_ROSPIXELCODE_H

namespace yarp {
    namespace dev {
        namespace ROSPixelCode {

#define BAYER_BGGR16 "bayer_bggr16"
#define BAYER_BGGR8  "bayer_bggr8"
#define BAYER_GBRG16 "bayer_gbrg16"
#define BAYER_GBRG8  "bayer_gbrg8"
#define BAYER_GRBG16 "bayer_grbg16"
#define BAYER_GRBG8  "bayer_grbg8"
#define BAYER_RGGB16 "bayer_rggb16"
#define BAYER_RGGB8  "bayer_rggb8"
#define BGR16        "bgr16"
#define BGR8         "bgr8"
#define BGRA16       "bgra16"
#define BGRA8        "bgra8"
#define MONO16       "mono16"
#define MONO8        "mono8"
#define RGB16        "rgb16"
#define RGB8         "rgb8"
#define RGBA16       "rgba16"
#define RGBA8        "rgba8"
#define TYPE_16SC1   "16SC1"
#define TYPE_16SC2   "16SC2"
#define TYPE_16SC3   "16SC3"
#define TYPE_16SC4   "16SC4"
#define TYPE_16UC1   "16UC1"
#define TYPE_16UC2   "16UC2"
#define TYPE_16UC3   "16UC3"
#define TYPE_16UC4   "16UC4"
#define TYPE_32FC1   "32FC1"
#define TYPE_32FC2   "32FC2"
#define TYPE_32FC3   "32FC3"
#define TYPE_32FC4   "32FC4"
#define TYPE_32SC1   "32SC1"
#define TYPE_32SC2   "32SC2"
#define TYPE_32SC3   "32SC3"
#define TYPE_32SC4   "32SC4"
#define TYPE_64FC1   "64FC1"
#define TYPE_64FC2   "64FC2"
#define TYPE_64FC3   "64FC3"
#define TYPE_64FC4   "64FC4"
#define TYPE_8SC1    "8SC1"
#define TYPE_8SC2    "8SC2"
#define TYPE_8SC3    "8SC3"
#define TYPE_8SC4    "8SC4"
#define TYPE_8UC1    "8UC1"
#define TYPE_8UC2    "8UC2"
#define TYPE_8UC3    "8UC3"
#define TYPE_8UC4    "8UC4"
#define YUV422       "yuv422"

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

int Ros2YarpPixelCode(std::string roscode)
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

#endif // YARP_DEV_RGBDSENSORWRAPPER_ROSPIXELCODE_H
