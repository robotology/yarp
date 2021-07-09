/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    size_t bytesPerSample = 2;
};

/**
* \brief `AudioPlayerDeviceBase` : a base class for all audio player devices
*
* Parameters required by this device are:
* | Parameter name   | SubParameter           | Type    | Units          | Default Value            | Required                    | Description                                                       | Notes |
* |:----------------:|:----------------------:|:-------:|:--------------:|:------------------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | AUDIO_BASE       | rate                   | int     | Hz             | 44100                    | No                          | The frequency of the audio device                                 | - |
* | AUDIO_BASE       | samples                | int     | samples        | 44100                    | No                          | The size of the internal buffer (in samples) | A default value identical to device frequency implies a buffer length of 1s |
* | AUDIO_BASE       | channels               | int     | -              | 2                        | No                          | The number of channels | - |
* | AUDIO_BASE       | bits                   | int     | -              | 16                       | No                          | Not yet implemented | - |
* | AUDIO_BASE       | hw_gain                | double  | -              | 1.0                      | No                          | The device audio gain. Its implementation is device dependent (and it may also be not implemented)  | - |
* | AUDIO_BASE       | sw_gain                | double  | -              | 1.0                      | No                          | A SW gain for audio waveform amplification | - |
* | AUDIO_BASE       | render_mode_append     | bool    | -              | true                     | No                          | A new sound is appended to the playback buffer and reproduced accordingly | - |
* | AUDIO_BASE       | render_mode_immediate  | bool    | -              | false                    | No                          | Every new sound is immediately reproduced, stopping the current playback | - |
* | AUDIO_BASE       | buffer_autoclear       | bool    | -              | false                    | No                          | Automatically clear the buffer every time the devices is started/stopped | If false, stop acts like a pause, and start allows to resume the playback |
* | AUDIO_BASE       | debug                  | bool    | -              | false                    | No                          | Enable debug mode | The value is stored into variable m_audiobase_debug |
*
* See \ref AudioDoc for additional documentation on YARP audio.
*/

class YARP_dev_API AudioPlayerDeviceBase : public yarp::dev::IAudioRender
{
protected:
    bool                                m_enable_buffer_autoclear = false;
    bool                                m_playback_enabled = false;
    std::recursive_mutex                m_mutex;
    yarp::dev::CircularAudioBuffer_16t* m_outputBuffer = nullptr;
    AudioDeviceDriverSettings           m_audioplayer_cfg;
    double                              m_sw_gain = 1.0;
    double                              m_hw_gain = 1.0;
    bool                                m_audiobase_debug = false;
    enum { RENDER_APPEND = 0, RENDER_IMMEDIATE = 1 } m_renderMode= RENDER_APPEND;

public:
    virtual bool renderSound(const yarp::sig::Sound& sound) override;
    virtual bool startPlayback() override;
    virtual bool stopPlayback() override;
    virtual bool isPlaying(bool& playback_enabled) override;
    virtual bool getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool resetPlaybackAudioBuffer() override;
    virtual bool setSWGain(double gain) override;

    virtual ~AudioPlayerDeviceBase();

protected:
    virtual bool configureDeviceAndStart() = 0;
    virtual bool interruptDeviceAndClose() = 0;
    virtual void waitUntilPlaybackStreamIsComplete();
    virtual bool immediateSound(const yarp::sig::Sound& sound);
    virtual bool appendSound(const yarp::sig::Sound& sound);

    bool configurePlayerAudioDevice(yarp::os::Searchable& config, std::string device_name);
};

} // namespace dev
} // namespace yarp

#endif
