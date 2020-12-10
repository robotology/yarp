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

#define DEFAULT_PERIOD 0.01   //s

/**
* @ingroup dev_impl_fake dev_impl_media
*
* \brief `fakeMicrophone` : fake microphone device implementing the IAudioGrabberSound interface to generate a test sound.
* It can generate various signals, i.e. sine, sawtooth, square wave, constant.
*
* Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units          | Default Value            | Required                    | Description                                                       | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:------------------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | period         |      -         | double  | s              |  0.010                   | No                          | the period of processing thread                                   | A value of 10ms is recommended. Do to not modify it |
* | channels       |      -         | size_t  | -              | 2                        | No                          | Number of channels (e.g. 1=mono, 2-stereo etc)                    | - |
* | waveform       |      -         | string  | -              | sine                     | No                          | Defines the shape of the waveform. Can be one of the following: sine,sawtooth,square,constant | - |
* | sampling_frequency |      -     | int     | Hz             | 44100                    | No                          | Sampling frequency | - |
* | signal_frequency   |      -     | int     | Hz             | 440                      | No                          | Frequency of the generated signal | - |

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
    size_t m_wave_amplitude = 32000;
    double m_sig_freq=440; //Hz

    enum waveform_t
    {
        sine = 0,
        sawtooth = 1,
        square = 2,
        constant = 3
    } m_waveform = sine;
};
