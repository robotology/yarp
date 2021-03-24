/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

namespace yarp yarp.dev

typedef i32 ( yarp.type = "size_t" ) size_t

struct audioPlayerStatus
{
    /** true if the playback is currently enabled */
    1: bool         enabled;

    /** the size of the audio buffer [samples] */
    2: size_t       current_buffer_size;

    /** the max_size of the audio buffer [samples] */
    3: size_t       max_buffer_size;
}
(
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
