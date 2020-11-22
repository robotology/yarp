/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IAudioRender.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>

#include <string>
#include <mutex>
#include <deque>

/**
* @ingroup dev_impl_media
*
* \brief `audioToFileDevice` : This device driver, wrapped by default by AudioPlayerWrapper,
* is used to save to a file an audio stream. Use the option --file_name to set the output file name
*/

class audioToFileDevice :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IAudioRender
{
public:
    audioToFileDevice();
    audioToFileDevice(const audioToFileDevice&) = delete;
    audioToFileDevice(audioToFileDevice&&) = delete;
    audioToFileDevice& operator=(const audioToFileDevice&) = delete;
    audioToFileDevice& operator=(audioToFileDevice&&) = delete;
    ~audioToFileDevice() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

public:
    virtual bool renderSound(const yarp::sig::Sound& sound) override;
    virtual bool startPlayback() override;
    virtual bool stopPlayback()override;
    virtual bool getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    virtual bool resetPlaybackAudioBuffer() override;

private:
    yarp::sig::Sound m_audioFile;
    std::string      m_audio_filename;
    std::mutex       m_mutex;
    bool             m_playback_running = false;
    std::deque<yarp::sig::Sound> m_sounds;
};
