/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/AudioBufferSize.h>

yarp::dev::AudioBufferSize::AudioBufferSize() = default;

yarp::dev::AudioBufferSize::AudioBufferSize(size_t samples, size_t channels, size_t depth_in_bytes)
{
    this->m_data.m_samples=samples;
    this->m_data.m_channels = channels;
    this->m_data.m_depth = depth_in_bytes;
    this->m_data.size = samples * channels;
}
