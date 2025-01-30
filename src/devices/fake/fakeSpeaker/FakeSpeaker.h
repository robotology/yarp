/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>
#include <yarp/dev/AudioPlayerDeviceBase.h>

#include "FakeSpeaker_ParamsParser.h"

/**
* @ingroup dev_impl_fake dev_impl_media
*
* \brief `fakeSpeaker` : fake device implementing the IAudioRender device interface to play sound
*
* This device driver derives from AudioPlayerDeviceBase base class. Please check its documentation for additional details.
*
* Parameters required by this device are shown in class: FakeMicrophone_ParamsParser
* This device also inherits some parameters from AudioPlayerDeviceBase
*
* See \ref AudioDoc for additional documentation on YARP audio.
*/

class FakeSpeaker :
        public yarp::dev::DeviceDriver,
        public yarp::dev::AudioPlayerDeviceBase,
        public yarp::os::PeriodicThread,
        public FakeSpeaker_ParamsParser
{
public:
    FakeSpeaker();
    FakeSpeaker(const FakeSpeaker&) = delete;
    FakeSpeaker(FakeSpeaker&&) = delete;
    FakeSpeaker& operator=(const FakeSpeaker&) = delete;
    FakeSpeaker& operator=(FakeSpeaker&&) = delete;
    ~FakeSpeaker() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

    //interface
    virtual yarp::dev::ReturnValue setHWGain(double gain) override;
    virtual bool configureDeviceAndStart() override;
    virtual bool interruptDeviceAndClose() override;

private:
    bool threadInit() override;
    void run() override;
};
