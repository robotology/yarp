/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IAudioRender.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>
#include <yarp/dev/AudioPlayerDeviceBase.h>
#include "AudioToFileDevice_ParamsParser.h"

#include <string>
#include <mutex>
#include <deque>

/**
* @ingroup dev_impl_media
*
* \brief `audioToFileDevice` : This device driver, wrapped by default by AudioPlayerWrapper,
* is used to save to an audio stream to a file on disk.
*
* This device driver derives from yarp::dev::AudioPlayerDeviceBase base class.
* Please check its documentation for additional configuration parameters.
*
* Parameters required by this device are shown in class: AudioToFileDevice_ParamsParser and AudioRecorderDeviceBase
*
* Three different operating modes are available, defined by the optional string parameter `save_mode`:
* if save_mode == "append_data", the file is written only when the module terminates.
* Every start/stop operation just pauses the module. On resume, the new data is concatenated at the end of the file.
*
* if save_mode == "overwrite_file", the output file is written every time the stop() method is called or when the module terminates.
* If the file already exists, it will be overwritten with the new data.
*
* if save_mode = "rename_file", the output file is written to a NEW file every time the stop() method is called or when the module terminates.
* The file name is modified, using an incremental counter appended at the end of the file name.
*
* if save_mode = "break_file", the output file is written to a NEW file every time a yarp::sig::sound is received or when the module terminates.
* The file name is modified, using an incremental counter appended at the end of the file name.

* See \ref AudioDoc for additional documentation on YARP audio.
*
*/

class AudioToFileDevice :
        public yarp::dev::DeviceDriver,
        public yarp::dev::AudioPlayerDeviceBase,
        public AudioToFileDevice_ParamsParser
{
public:
    AudioToFileDevice();
    AudioToFileDevice(const AudioToFileDevice&) = delete;
    AudioToFileDevice(AudioToFileDevice&&) = delete;
    AudioToFileDevice& operator=(const AudioToFileDevice&) = delete;
    AudioToFileDevice& operator=(AudioToFileDevice&&) = delete;
    ~AudioToFileDevice() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

public:
    virtual yarp::dev::ReturnValue renderSound(const yarp::sig::Sound& sound) override;
    virtual yarp::dev::ReturnValue startPlayback() override;
    virtual yarp::dev::ReturnValue stopPlayback()override;
    virtual yarp::dev::ReturnValue setHWGain(double gain) override;
    virtual bool configureDeviceAndStart() override;
    virtual bool interruptDeviceAndClose() override;
    virtual void waitUntilPlaybackStreamIsComplete() override;

private:
    yarp::sig::Sound m_audioFile;
    std::deque<yarp::sig::Sound> m_sounds;
    size_t           m_filename_counter = 0;

    enum save_mode_t
    {
        save_overwrite_file = 0,
        save_append_data =1,
        save_rename_file =2,
        save_break_file =3
    } m_save_mode_enum = save_overwrite_file;

    void save_to_file();
};
