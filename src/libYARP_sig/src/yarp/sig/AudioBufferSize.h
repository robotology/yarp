/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_AUDIOBUFFERSIZE_H
#define YARP_SIG_AUDIOBUFFERSIZE_H

#include <yarp/os/Portable.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>
#include <yarp/sig/api.h>
#include <yarp/sig/audioBufferSizeData.h>

namespace yarp::sig {

template <typename SAMPLE>
class CircularAudioBuffer;

class YARP_sig_API AudioBufferSize :
    public yarp::os::idl::WirePortable
{
private:
    audioBufferSizeData m_data;

public:
    size_t getDepth() { return m_data.m_depth; }
    size_t getSamples() { return m_data.m_samples; }
    size_t getChannels() { return m_data.m_channels; }
    size_t getBufferElements() { return m_data.size; }
    size_t getBytes() { return size_t(m_data.m_samples * m_data.m_channels * m_data.m_depth); }

    bool read(yarp::os::idl::WireReader& reader) override { return m_data.read(reader); }
    bool write(const yarp::os::idl::WireWriter& writer) const override { return m_data.write(writer); }
    bool read(yarp::os::ConnectionReader& reader) override { return m_data.read(reader); }
    bool write(yarp::os::ConnectionWriter& writer) const override { return m_data.write(writer); }

    AudioBufferSize();
    AudioBufferSize(size_t samples, size_t channels, size_t depth_in_bytes);
};

} // namespace yarp::sig

#endif // YARP_SIG_AUDIOBUFFERSIZE_H
