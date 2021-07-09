/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vector>

#include <yarp/os/Searchable.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/api.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <mutex>

#ifndef YARP_DEV_AUDIORECORDERDEVICETEMPLATE_H
#define YARP_DEV_AUDIORECORDERDEVICETEMPLATE_H

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
* \brief `AudioRecorderDeviceBase` : a base class for all audio recorder devices
*
* Parameters required by this device are:
* | Parameter name   | SubParameter      | Type    | Units          | Default Value            | Required                    | Description                                                       | Notes |
* |:----------------:|:-----------------:|:-------:|:--------------:|:------------------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | AUDIO_BASE       |  rate             | int     | Hz             | 44100                    | No                          | The frequency of the audio device                                 | - |
* | AUDIO_BASE       |  samples          | int     | samples        | 44100                    | No                          | The size of the internal buffer (in samples) | A default value identical to device frequency implies a buffer length of 1s |
* | AUDIO_BASE       |  channels         | int     | -              | 2                        | No                          | The number of channels | - |
* | AUDIO_BASE       |  bits             | int     | -              | 16                       | No                          | Not yet implemented | - |
* | AUDIO_BASE       |  hw_gain          | double  | -              | 1.0                      | No                          | The device audio gain. Its implementation is device dependent (and it may also be not implemented)  | - |
* | AUDIO_BASE       |  sw_gain          | double  | -              | 1.0                      | No                          | A SW gain for audio waveform amplification | - |
* | AUDIO_BASE       |  buffer_autoclear | bool    | -              | true                     | No                          | Automatically clear the buffer every time the devices is started/stopped | - |
* | AUDIO_BASE       |  debug            | bool    | -              | false                    | No                          | Enable debug mode | The value is stored into variable m_audiobase_debug |
*
* See \ref AudioDoc for additional documentation on YARP audio.
*/

class YARP_dev_API AudioRecorderDeviceBase : public yarp::dev::IAudioGrabberSound
{
protected:
    bool            m_enable_buffer_autoclear = false;
    bool            m_recording_enabled = false;
    std::mutex      m_mutex;
    yarp::dev::CircularAudioBuffer_16t* m_inputBuffer = nullptr;
    double          m_sw_gain = 1.0;
    double          m_hw_gain = 1.0;
    AudioDeviceDriverSettings m_audiorecorder_cfg;
    bool            m_audiobase_debug = false;
    int16_t         m_cliptol = 3;

public:
    virtual bool getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s) override;
    virtual bool startRecording() override;
    virtual bool stopRecording() override;
    virtual bool isRecording(bool& recording_enabled) override;
    virtual bool getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool resetRecordingAudioBuffer() override;
    virtual bool setSWGain(double gain) override;

    virtual ~AudioRecorderDeviceBase();

protected:
    bool configureRecorderAudioDevice(yarp::os::Searchable& config, std::string device_name);
};

} // namespace dev
} // namespace yarp

#endif
