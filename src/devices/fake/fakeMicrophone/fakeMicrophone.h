/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
* @ingroup dev_impl_fake dev_impl_media
*
* \brief `fakeMicrophone` : fake microphone device implementing the IAudioGrabberSound interface to generate a test sound.
* It can generate various signals, i.e. sine, sawtooth, square wave, constant.
* This device driver derives from AudioRecorderDeviceBase base class. Please check its documentation for additional details.
*
* Parameters used by this device are:
* | Parameter name    | SubParameter   | Type    | Units          | Default Value            | Required                    | Description                                                       | Notes |
* |:-----------------:|:--------------:|:-------:|:--------------:|:------------------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | AUDIO_BASE        |     ***        |         | -              |  -                       | No                          | For the documentation of AUDIO_BASE group, please refer to the documentation of the base class AudioRecorderDeviceBase |       |
* | period            |      -         | double  | s              |  0.010                   | No                          | the period of processing thread                                   | A value of 10ms is recommended. Do to not modify it |
* | waveform          |      -         | string  | -              | sine                     | No                          | Defines the shape of the waveform. Can be one of the following: sine,sawtooth,square,constant | - |
* | signal_frequency  |      -         | int     | Hz             | 440                      | No                          | Frequency of the generated signal | - |
* | signal_amplitude  |      -         | int     |                | 32000                    | No                          | Amplitude of the generated signal | - |
* | driver_frame_size |      -         | int     | samples        |  512                     | No                          | the number of samples to process on each iteration of the thread  | - |
*
* See \ref AudioDoc for additional documentation on YARP audio.
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
    size_t m_wave_amplitude = 32000;
    double m_sig_freq=440; //Hz
    size_t m_samples_to_be_copied = 512;

    enum waveform_t
    {
        sine = 0,
        sawtooth = 1,
        square = 2,
        constant = 3
    } m_waveform = sine;
};
