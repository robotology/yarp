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

#include <yarp/os/Portable.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>
#include <yarp/dev/api.h>
#include <audioBufferSizeData.h>

namespace yarp {
    namespace dev {
        class AudioBufferSize;

        template <typename SAMPLE>
        class CircularAudioBuffer;
    }
}

class YARP_dev_API yarp::dev::AudioBufferSize: //public yarp::os::Portable,
                                               private audioBufferSizeData
{
    template <typename SAMPLE>
    friend class CircularAudioBuffer;

    public:
    size_t getSamples() { return m_samples; }
    size_t getChannels() { return m_channels; }
    size_t getBufferElements() { return size; }
    size_t getBytes() { return m_samples * m_channels * m_depth; }

    virtual bool read(yarp::os::idl::WireReader& reader) override { return audioBufferSizeData::read(reader); }
    virtual bool write(const yarp::os::idl::WireWriter& writer) const override { return audioBufferSizeData::write(writer); }
    virtual bool read(yarp::os::ConnectionReader& reader) override { return audioBufferSizeData::read(reader); }
    virtual bool write(yarp::os::ConnectionWriter& writer) const override { return audioBufferSizeData::write(writer); }

    AudioBufferSize()
    {
        size = 0;
        m_samples = 0;
        m_channels = 0;
        m_depth = 0;
    }
    AudioBufferSize(size_t samples, size_t channels, size_t depth_in_bytes)
    {
        //this casts are due to the fact the it is not yet possibile to define an unsigned type in thrift
        m_samples = (int32_t)(samples);
        m_channels = (int32_t)(channels);
        m_depth = (int32_t)(depth_in_bytes);
        size = (int32_t)(samples * channels);
    }
};


#endif // YARP_DEV_AUDIOBUFFERSIZE_H
