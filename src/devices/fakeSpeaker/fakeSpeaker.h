/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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
#include <yarp/dev/AudioPlayerDeviceBase.h>


#define DEFAULT_PERIOD 0.01   //s

/**
 * @ingroup dev_impl_fake dev_impl_media
 *
 * \brief `fakeSpeaker` : fake device implementing the IAudioRender device interface to play sound
 *
 * Documentation to be added
 */
class fakeSpeaker :
        public yarp::dev::DeviceDriver,
        public yarp::dev::AudioPlayerDeviceBase,
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
    virtual bool setHWGain(double gain) override;

private:
    bool threadInit() override;
    void run() override;

    size_t m_cfg_numSamples = 0;
    size_t m_cfg_numChannels = 0;
    size_t m_cfg_frequency = 0;
    size_t m_cfg_bytesPerSample = 0;
    double m_hw_gain = 1.0;

    bool m_renderSoundImmediate = false;
};
