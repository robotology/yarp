/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/IGenericSensor.h>
#include <yarp/dev/AudioRecorderDeviceBase.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>

#include <string>
#include <mutex>

/**
* @ingroup dev_impl_media
*
* \brief `audioFromFileDevice` : This device driver, wrapped by default by AudioRecorderWrapper,
* is used to read data from a file and stream it to the network.
* This device driver derives from AudioRecorderDeviceBase base class. Please check its documentation for additional details.
*
* Parameters required by this device are:
* | Parameter name    | SubParameter   | Type    | Units          | Default Value            | Required                    | Description                                                       | Notes |
* |:-----------------:|:--------------:|:-------:|:--------------:|:------------------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | AUDIO_BASE        |     ***        |         | -              |  -                       | No                          | For the documentation of AUDIO_BASE group, please refer to the documentation of the base class AudioRecorderDeviceBase |       |
* | file_name         |      -         | string  | -              |  audio.wav               | No                          | The name of the file opened by the module                         | Only .wav files supported   |
* | period            |      -         | double  | s              |  0.010                   | No                          | the period of thread which processes the file                     | On each iteration xxx samples are processed |
* | driver_frame_size |      -         | int     | samples        |  512                     | No                          | the number of samples to process on each iteration of the thread  | - |
*/

class audioFromFileDevice :
        public yarp::dev::DeviceDriver,
        public yarp::dev::AudioRecorderDeviceBase,
        public yarp::os::PeriodicThread
{
public:
    audioFromFileDevice();
    audioFromFileDevice(const audioFromFileDevice&) = delete;
    audioFromFileDevice(audioFromFileDevice&&) = delete;
    audioFromFileDevice& operator=(const audioFromFileDevice&) = delete;
    audioFromFileDevice& operator=(audioFromFileDevice&&) = delete;
    ~audioFromFileDevice() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

private:
   //thread
    bool threadInit() override;
    void run() override;

public:
    bool setHWGain(double gain) override;

private:
    yarp::sig::Sound m_audioFile;
    std::string m_audio_filename = "audio.wav";
    size_t m_bpnt = 0;
    size_t m_samples_to_be_copied = 512;
    size_t m_fsize_in_samples = 0;
    std::vector<std::reference_wrapper<yarp::sig::Sound::audio_sample>> m_datap;
};
