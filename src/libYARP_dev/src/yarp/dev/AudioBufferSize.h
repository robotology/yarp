/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_AUDIOBUFFERSIZE_H
#define YARP_DEV_AUDIOBUFFERSIZE_H

#include <yarp/os/Portable.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>
#include <yarp/dev/api.h>
#include <yarp/dev/audioBufferSizeData.h>

namespace yarp {
namespace dev {

template <typename SAMPLE>
class CircularAudioBuffer;

class YARP_dev_API AudioBufferSize :
        private audioBufferSizeData
{
    template <typename SAMPLE>
    friend class CircularAudioBuffer;

public:
    size_t getSamples() { return m_samples; }
    size_t getChannels() { return m_channels; }
    size_t getBufferElements() { return size; }
    size_t getBytes() { return size_t(m_samples * m_channels * m_depth); }

    bool read(yarp::os::idl::WireReader& reader) override { return audioBufferSizeData::read(reader); }
    bool write(const yarp::os::idl::WireWriter& writer) const override { return audioBufferSizeData::write(writer); }
    bool read(yarp::os::ConnectionReader& reader) override { return audioBufferSizeData::read(reader); }
    bool write(yarp::os::ConnectionWriter& writer) const override { return audioBufferSizeData::write(writer); }

    AudioBufferSize();
    AudioBufferSize(size_t samples, size_t channels, size_t depth_in_bytes);
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_AUDIOBUFFERSIZE_H
