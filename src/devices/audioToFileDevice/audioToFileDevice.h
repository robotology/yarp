/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IAudioRender.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>
#include <yarp/dev/AudioPlayerDeviceBase.h>
#include "audioToFileDeviceParams.h"

#include <string>
#include <mutex>
#include <deque>

/**
* @ingroup dev_impl_media
*
* \brief `audioToFileDevice` : This device driver, wrapped by default by AudioPlayerWrapper,
* is used to save to an audio stream to a file on disk.
*
* See audioToFileDeviceParams for the documentation of the accepted parameters.
*
* This device driver derives from yarp::dev::AudioPlayerDeviceBase base class.
* Please check its documentation for additional configuration parameters.
*
* See \ref AudioDoc for additional documentation on YARP audio.
*/

class audioToFileDevice :
        public yarp::dev::DeviceDriver,
        public yarp::dev::AudioPlayerDeviceBase,
        public audioToFileDevice_params
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
    virtual bool setHWGain(double gain) override;
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
    } m_save_mode = save_overwrite_file;

    void save_to_file();
};
