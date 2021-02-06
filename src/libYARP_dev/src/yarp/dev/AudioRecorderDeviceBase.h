/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    size_t bytesPerSample = 0;
};

class YARP_dev_API AudioRecorderDeviceBase : public yarp::dev::IAudioGrabberSound
{
protected:
    bool m_isRecording = false;
    std::mutex  m_mutex;
    yarp::dev::CircularAudioBuffer_16t* m_inputBuffer = nullptr;

    AudioDeviceDriverSettings m_audiorecorder_cfg;

public:
    virtual bool getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s) override;
    virtual bool startRecording() override;
    virtual bool stopRecording() override;
    virtual bool getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool resetRecordingAudioBuffer() override;
    virtual bool setSWGain(double gain) override;

    virtual ~AudioRecorderDeviceBase();
};

} // namespace dev
} // namespace yarp

#endif
