/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <vector>

#include <yarp/os/Searchable.h>
#include <yarp/dev/IAudioRender.h>
#include <yarp/dev/api.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <mutex>

#ifndef YARP_DEV_AUDIOPLAYERDEVICETEMPLATE_H
#define YARP_DEV_AUDIOPLAYERDEVICETEMPLATE_H

namespace yarp {
namespace dev {


class AudioDeviceDriverSettings
{
    public:
    size_t numSamples = 0;
    size_t numChannels = 0;
    size_t frequency = 0;
    size_t bytesPerSample = 0;
};

class YARP_dev_API AudioPlayerDeviceBase : public yarp::dev::IAudioRender
{
protected:
    bool m_isPlaying = false;
    std::mutex  m_mutex;
    yarp::dev::CircularAudioBuffer_16t* m_outputBuffer = nullptr;

    AudioDeviceDriverSettings m_audioplayer_cfg;

public:
    virtual bool renderSound(const yarp::sig::Sound& sound) override;
    virtual bool startPlayback() override;
    virtual bool stopPlayback() override;
    virtual bool getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool resetPlaybackAudioBuffer() override;
    virtual bool setSWGain(double gain) override;

    virtual ~AudioPlayerDeviceBase();
};

} // namespace dev
} // namespace yarp

#endif
