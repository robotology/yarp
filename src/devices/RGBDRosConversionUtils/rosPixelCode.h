/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_RGBDSENSORWRAPPER_ROSPIXELCODE_H
#define YARP_DEV_RGBDSENSORWRAPPER_ROSPIXELCODE_H

#include <string>
#include <yarp/sig/Image.h>

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

std::string yarp2RosPixelCode(int code);

int Ros2YarpPixelCode(const std::string& roscode);

}}}

#endif // YARP_DEV_RGBDSENSORWRAPPER_ROSPIXELCODE_H
