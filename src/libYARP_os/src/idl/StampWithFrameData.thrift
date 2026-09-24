/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.os

/**
 * @brief Holds a time stamp and/or sequence number.
 */
struct StampWithFrameData
{
  1: i32     sequenceNumber;
  2: double  timeStamp;
  3: string  frameId;
} (
  yarp.data_visibility = "public"
  yarp.api.include = "yarp/os/api.h"
  yarp.api.keyword = "YARP_os_API"
)
