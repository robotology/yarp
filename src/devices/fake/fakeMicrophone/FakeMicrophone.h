/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/AudioRecorderDeviceBase.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>

#include <string>
#include <mutex>

#include "FakeMicrophone_ParamsParser.h"

/**
* @ingroup dev_impl_fake dev_impl_media
*
* \brief `fakeMicrophone` : fake microphone device implementing the IAudioGrabberSound interface to generate a test sound.
* It can generate various signals, i.e. sine, sawtooth, square wave, constant.
* This device driver derives from AudioRecorderDeviceBase base class. Please check its documentation for additional details.
*
* Parameters required by this device are shown in class: FakeMicrophone_ParamsParser
* This device also inherits some parameters from AudioRecorderDeviceBase
*
* See \ref AudioDoc for additional documentation on YARP audio.
*/

class FakeMicrophone :
        public yarp::dev::DeviceDriver,
        public yarp::dev::AudioRecorderDeviceBase,
        public yarp::os::PeriodicThread,
        public FakeMicrophone_ParamsParser
{
public:
    FakeMicrophone();
    FakeMicrophone(const FakeMicrophone&) = delete;
    FakeMicrophone(FakeMicrophone&&) = delete;
    FakeMicrophone& operator=(const FakeMicrophone&) = delete;
    FakeMicrophone& operator=(FakeMicrophone&&) = delete;
    virtual ~FakeMicrophone() override;

public:
    bool setHWGain(double gain) override;

public: // DeviceDriver
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

private:
   //thread
    bool threadInit() override;
    void run() override;

private:
    std::vector<size_t> m_counter;
    std::vector<size_t> m_max_count;

    enum waveform_t
    {
        sine = 0,
        sawtooth = 1,
        square = 2,
        constant = 3
    } m_waveform_enum = sine;
};
