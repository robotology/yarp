/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <string>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/IGenericSensor.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>


#define DEFAULT_PERIOD 0.01   //s

/**
* \brief `fakeSpeaker` : fake device implementing the IAudioRender device interface to play sound
*
*/
class fakeSpeaker :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IAudioRender,
        public yarp::os::PeriodicThread
{
public:
    fakeSpeaker();
    fakeSpeaker(const fakeSpeaker&) = delete;
    fakeSpeaker(fakeSpeaker&&) = delete;
    fakeSpeaker& operator=(const fakeSpeaker&) = delete;
    fakeSpeaker& operator=(fakeSpeaker&&) = delete;

    ~fakeSpeaker() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

    virtual bool startPlayback() override;
    virtual bool stopPlayback() override;
    virtual bool renderSound(const yarp::sig::Sound& sound)  override;
    virtual bool getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)  override;
    virtual bool getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)  override;
    virtual bool resetPlaybackAudioBuffer() override;

private:
    bool threadInit() override;
    void run() override;

    bool m_isPlaying = false;

    size_t m_cfg_numSamples = 0;
    size_t m_cfg_numChannels = 0;
    size_t m_cfg_frequency = 0;
    size_t m_cfg_bytesPerSample = 0;

    yarp::dev::CircularAudioBuffer_16t* m_outputBuffer = nullptr;
    bool m_renderSoundImmediate = false;
};
