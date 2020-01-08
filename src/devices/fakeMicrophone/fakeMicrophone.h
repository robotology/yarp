/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/IGenericSensor.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>

#include <string>
#include <mutex>

#define DEFAULT_PERIOD 0.01   //s

/**
* \brief `fakeMicrophone` : fake device implementing the IAudioGrabberSound device interface to play sound
*
*/
class fakeMicrophone :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IAudioGrabberSound,
        public yarp::os::PeriodicThread
{
public:
    fakeMicrophone();
    fakeMicrophone(const fakeMicrophone&) = delete;
    fakeMicrophone(fakeMicrophone&&) = delete;
    fakeMicrophone& operator=(const fakeMicrophone&) = delete;
    fakeMicrophone& operator=(fakeMicrophone&&) = delete;

    ~fakeMicrophone() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

    virtual bool getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s) override;
    virtual bool startRecording() override;
    virtual bool stopRecording() override;
    virtual bool getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool resetRecordingAudioBuffer() override;

private:
    bool threadInit() override;
    void run() override;

    bool m_isRecording;
    std::mutex  m_mutex;
    yarp::sig::Sound m_audioFile;

    size_t m_cfg_numSamples;
    size_t m_cfg_numChannels;
    size_t m_cfg_frequency;
    size_t m_cfg_bytesPerSample;

    std::string m_audio_filename;
    size_t m_bpnt;
    yarp::dev::CircularAudioBuffer_16t* m_inputBuffer;
    bool m_getSoundIsNotBlocking;
};
