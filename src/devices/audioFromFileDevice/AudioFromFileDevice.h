/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/AudioRecorderDeviceBase.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>
#include "AudioFromFileDevice_ParamsParser.h"

#include <string>
#include <mutex>
#include <vector>
#include <functional>

/**
* @ingroup dev_impl_media
*
* \brief `audioFromFileDevice` : This device driver, wrapped by default by AudioRecorderWrapper,
* is used to read data from a file and stream it to the network.
* This device driver derives from \ref AudioRecorderDeviceBase base class.
* Please check its documentation for additional details.
*
* Parameters required by this device are shown in class: AudioFromFileDevice_ParamsParser and AudioRecorderDeviceBase
*
* See \ref AudioDoc for additional documentation on YARP audio.
*/

class AudioFromFileDevice :
        public yarp::dev::DeviceDriver,
        public yarp::dev::AudioRecorderDeviceBase,
        public yarp::os::PeriodicThread,
        public AudioFromFileDevice_ParamsParser
{
public:
    AudioFromFileDevice();
    AudioFromFileDevice(const AudioFromFileDevice&) = delete;
    AudioFromFileDevice(AudioFromFileDevice&&) = delete;
    AudioFromFileDevice& operator=(const AudioFromFileDevice&) = delete;
    AudioFromFileDevice& operator=(AudioFromFileDevice&&) = delete;
    ~AudioFromFileDevice() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

private:
   //thread
    bool threadInit() override;
    void run() override;

public:
    bool setHWGain(double gain) override;
    bool stopRecording () override;

private:
    yarp::sig::Sound m_audioFile;
    size_t m_bpnt = 0;
    std::vector<std::reference_wrapper<yarp::sig::Sound::audio_sample>> m_datap;
};
