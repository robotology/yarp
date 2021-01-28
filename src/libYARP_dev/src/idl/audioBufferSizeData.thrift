/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

namespace yarp yarp.dev

struct audioBufferSizeData
{
    1: i32 m_samples;
    2: i32 m_channels;
    3: i32 m_depth;
    4: i32 size;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
