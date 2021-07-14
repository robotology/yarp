/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

typedef i32 ( yarp.type = "size_t" ) size_t

/**
* \brief `AudioRecorderStatus`: A class used to describe the status of an audio recorder device.
* See \ref AudioDoc for additional documentation on YARP audio.
*/
struct AudioRecorderStatus
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
