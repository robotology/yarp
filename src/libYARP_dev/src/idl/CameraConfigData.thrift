/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

enum yVPixels{}(
  yarp.name = "YarpVocabPixelTypesEnum"
  yarp.includefile = "yarp/sig/Image.h"
  yarp.enumbase = "yarp::conf::vocab32_t"
)

struct CameraConfigData
{
  1: i32 width = 0
  2: i32 height = 0
  3: double framerate = 0.0
  4: yVPixels pixelCoding
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
