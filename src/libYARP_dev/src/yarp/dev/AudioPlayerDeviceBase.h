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
    bool                                m_isPlaying = false;
    std::mutex                          m_mutex;
    yarp::dev::CircularAudioBuffer_16t* m_outputBuffer = nullptr;
    bool                                m_something_to_play = false;
    AudioDeviceDriverSettings           m_audioplayer_cfg;
    enum { RENDER_APPEND = 0, RENDER_IMMEDIATE = 1 } m_renderMode= RENDER_APPEND;

public:
    virtual bool renderSound(const yarp::sig::Sound& sound) override;
    virtual bool startPlayback() override;
    virtual bool stopPlayback() override;
    virtual bool getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool resetPlaybackAudioBuffer() override;
    virtual bool setSWGain(double gain) override;

    virtual ~AudioPlayerDeviceBase();

protected:
    virtual bool configureDeviceAndStart() = 0;
    virtual bool interruptDeviceAndClose() = 0;
    virtual void waitUntilPlaybackStreamIsComplete() = 0;
    virtual bool immediateSound(const yarp::sig::Sound& sound);
    virtual bool appendSound(const yarp::sig::Sound& sound);

    bool configurePlayerAudioDevice(yarp::os::Searchable& config);
};

} // namespace dev
} // namespace yarp

#endif
