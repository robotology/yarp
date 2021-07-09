/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef PortAudioPlayerDeviceDriverh
#define PortAudioPlayerDeviceDriverh

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Thread.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/AudioPlayerDeviceBase.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <portaudio.h>
#include <mutex>

/**
 * @ingroup dev_impl_media
 *
 * \brief `portaudioPlayer`: A device driver for an audio playback device wrapped by PortAudio library.
 * Requires the PortAudio library (http://www.portaudio.com), at least v19.
 * Only 16bits sample format is currently supported by this device.
 * This device driver derives from AudioPlayerDeviceBase base class. Please check its documentation for additional details.
 *
 * Parameters used by this device are:
 * | Parameter name    | SubParameter   | Type    | Units          | Default Value            | Required                    | Description                                                       | Notes |
 * |:-----------------:|:--------------:|:-------:|:--------------:|:------------------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
 * | AUDIO_BASE        |     ***        |         | -              |  -                       | No                          | For the documentation of AUDIO_BASE group, please refer to the documentation of the base class AudioPlayerDeviceBase |       |
 * | id                |      -         | int     | -              |  -                       | No                          | The device id, if multiple sound cards are present                | if not specified, the default system device will be used |
 * | driver_frame_size |      -         | int     | samples        |  512                     | No                          | the number of samples to process on each iteration of the main thread  | - | *
 *
 * See \ref AudioDoc for additional documentation on YARP audio.
 */

class PortAudioPlayerDeviceDriver :
        public yarp::dev::AudioPlayerDeviceBase,
        public yarp::dev::DeviceDriver,
        public yarp::os::Thread
{
private:
    PaStreamParameters  m_outputParameters;
    PaStream*           m_stream;
    PaError             m_err;

public:
    PortAudioPlayerDeviceDriver();
    PortAudioPlayerDeviceDriver(const PortAudioPlayerDeviceDriver&) = delete;
    PortAudioPlayerDeviceDriver(PortAudioPlayerDeviceDriver&&) = delete;
    PortAudioPlayerDeviceDriver& operator=(const PortAudioPlayerDeviceDriver&) = delete;
    PortAudioPlayerDeviceDriver& operator=(PortAudioPlayerDeviceDriver&&) = delete;
    ~PortAudioPlayerDeviceDriver() override;

private:
    bool abortSound();
    bool configureDeviceAndStart() override;

public: //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

public: //AudioRecorderDeviceBase(IAudioGrabberSound)
    void waitUntilPlaybackStreamIsComplete() override;
    bool setHWGain(double gain) override;
    bool interruptDeviceAndClose() override;
    bool startPlayback() override;
    bool stopPlayback() override;

public: //Thread
    void threadRelease() override;
    bool threadInit() override;
    void run() override;

protected:
    void*   m_system_resource;

    int  m_device_id;
    int  m_driver_frame_size;
    void handleError();
};

#endif
