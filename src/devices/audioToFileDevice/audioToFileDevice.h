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
* is used to save to a file an audio stream.
*
* Three different operating modes are available, defined by the optional string parameter `save_mode`:
* if save_mode == "append_data", the file is written only when the module terminates.
* Every start/stop operation just pauses the module. On resume, the new data is concatenated at the end of the file.
*
* if save_mode == "overwrite_file", the output file is written every time the stop() method is called or when the module terminates.
* If the file already exists, it will be overwritten with the new data.
*
* if save_mode = "rename_file", the output file is written every time the stop() method is called or when the module terminates.
* The file name is modified, using an incremental counter appended at the end of the file name.
*
* Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units          | Default Value            | Required                    | Description                                                       | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:------------------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | file_name      |      -         | string  | -              |  audio_out.wav           | No                          | The name of the file written by the module                        | Only .wav files supported   |
* | save_mode      |      -         | string  | -              |  overwrite_file          | No                          | Affects the behavior of the module and defines the save mode, as described in the documentation.   |       |
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
    std::string      m_audio_filename = "audio_out.wav";
    std::mutex       m_mutex;
    bool             m_playback_running = false;
    std::deque<yarp::sig::Sound> m_sounds;
    size_t m_filename_counter = 0;
    enum save_mode_t
    {
        save_overwrite_file = 0,
        save_append_data =1,
        save_rename_file =2
    } m_save_mode = save_overwrite_file;

    void save_to_file();
};
