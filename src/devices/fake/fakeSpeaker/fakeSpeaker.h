/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

/**
* @ingroup dev_impl_fake dev_impl_media
*
* \brief `fakeSpeaker` : fake device implementing the IAudioRender device interface to play sound
*
* This device driver derives from AudioPlayerDeviceBase base class. Please check its documentation for additional details.
*
* Parameters used by this device are:
* | Parameter name   | SubParameter   | Type    | Units          | Default Value            | Required                    | Description                                                       | Notes |
* |:----------------:|:--------------:|:-------:|:--------------:|:------------------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | AUDIO_BASE       |     ***        |         | -              |  -                       | No                          | For the documentation of AUDIO_BASE group, please refer to the documentation of the base class AudioPlayerDeviceBase |       |
* | period           |      -         | double  | s              |  0.010                   | No                          | the period of processing thread                                   | A value of 10ms is recommended. Do to not modify it |
*
* See \ref AudioDoc for additional documentation on YARP audio.
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

    //interface
    virtual bool setHWGain(double gain) override;
    virtual bool configureDeviceAndStart() override;
    virtual bool interruptDeviceAndClose() override;

private:
    bool threadInit() override;
    void run() override;
};
