/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_AUDIOBUFFERSIZE_H
#define YARP_DEV_AUDIOBUFFERSIZE_H

namespace yarp {
    namespace dev {
        class AudioBufferSize;

        template <typename SAMPLE>
        class CircularAudioBuffer;
    }
}

class yarp::dev::AudioBufferSize
{
    template <typename SAMPLE>
    friend class CircularAudioBuffer;

    private:
    size_t m_samples;
    size_t m_channels;
    size_t m_depth;
    size_t size;

    public:
    size_t getSamples() { return m_samples; }
    size_t getChannels() { return m_channels; }
    size_t getBufferElements() { return size; }
    size_t getBytes() { return m_samples * m_channels * m_depth; }

    AudioBufferSize()
    {
        size = 0;
        m_samples = 0;
        m_channels = 0;
        m_depth = 0;
    }
    AudioBufferSize(size_t samples, size_t channels, size_t depth_in_bytes)
    {
        m_samples = samples;
        m_channels = channels;
        m_depth = depth_in_bytes;
        size = samples * channels;
    }
};


#endif // YARP_DEV_AUDIOBUFFERSIZE_H
