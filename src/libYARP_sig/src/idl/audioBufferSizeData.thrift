/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.sig

struct audioBufferSizeData
{
    1: i32 m_samples;
    2: i32 m_channels;
    3: i32 m_depth;
    4: i32 size;
} (
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)
