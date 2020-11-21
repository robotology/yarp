/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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

#define DEFAULT_PERIOD 0.01   //s

/**
* @ingroup dev_impl_fake dev_impl_media
*
* \brief `fakeMicrophone` : fake device implementing the IAudioGrabberSound device interface to play sound
*/
class fakeMicrophone :
        public yarp::dev::DeviceDriver,
        public yarp::dev::AudioRecorderDeviceBase,
        public yarp::os::PeriodicThread
{
public:
    fakeMicrophone();
    fakeMicrophone(const fakeMicrophone&) = delete;
    fakeMicrophone(fakeMicrophone&&) = delete;
    fakeMicrophone& operator=(const fakeMicrophone&) = delete;
    fakeMicrophone& operator=(fakeMicrophone&&) = delete;
    ~fakeMicrophone() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

private:
   //thread
    bool threadInit() override;
    void run() override;

private:
    yarp::sig::Sound m_audioFile;
    std::string m_audio_filename;
    size_t m_bpnt;
};
