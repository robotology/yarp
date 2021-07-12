/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/AudioBufferSize.h>

yarp::dev::AudioBufferSize::AudioBufferSize() = default;

//this casts are due to the fact the it is not yet possibile to define an unsigned type in thrift
yarp::dev::AudioBufferSize::AudioBufferSize(size_t samples, size_t channels, size_t depth_in_bytes) :
        audioBufferSizeData(static_cast<int32_t>(samples),
                            static_cast<int32_t>(channels),
                            static_cast<int32_t>(depth_in_bytes),
                            static_cast<int32_t>(samples * channels))
{
}
