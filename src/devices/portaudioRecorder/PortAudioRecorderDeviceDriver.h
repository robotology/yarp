/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PortAudioRecorderDeviceDriverh
#define PortAudioRecorderDeviceDriverh

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Thread.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/AudioRecorderDeviceBase.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <portaudio.h>

/**
* @ingroup dev_impl_media
*
* \brief `portaudioRecorder`: A device driver for an audio source wrapped by PortAudio library.
* Requires the PortAudio library (http://www.portaudio.com), at least v19.
* Only 16bits sample format is currently supported by this device.
*
* Parameters used by this device are:
* | Parameter name | SubParameter   | Type    | Units          | Default Value            | Required                    | Description                                                       | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:------------------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | rate           |      -         | int     | Hz             |  44100                   | No                          | bitrate / sampling frequency                                      |       |
* | samples        |      -         | int     | samples        |  44100                   | No                          | The size of the internal circular buffer                          | By default this value is equal to the sampling rate, so the buffer size is one second |
* | channels       |      -         | int     | -              |  2                       | No                          | Number of channels (e.g. 1=mono, 2 =stereo etc                    |       |
* | sample_format  |      -         | int     | bits           |  16 cannot be modified   | Not yet implemented         | Not yet implemented                                               |  Not yet implemented   |
* | driver_frame_size   |     -     | int     | samples        |  512                     | No                          | Number of samples grabbed by the device in a single uninterruptible operation |  It is recommended to NOT CHANGE this value from its default=512  |
* | id             |      -         | int     | -              |  -1                      | No                          | Id of the sound card.                                             | if == -1, portaudio will choose automatically  |
 */
class PortAudioRecorderDeviceDriver :
        public yarp::dev::AudioRecorderDeviceBase,
        public yarp::dev::DeviceDriver,
        public yarp::os::Thread
{
private:
    PaStreamParameters  m_inputParameters;
    PaStream*           m_stream;
    PaError             m_err;

public:
    PortAudioRecorderDeviceDriver();
    PortAudioRecorderDeviceDriver(const PortAudioRecorderDeviceDriver&) = delete;
    PortAudioRecorderDeviceDriver(PortAudioRecorderDeviceDriver&&) = delete;
    PortAudioRecorderDeviceDriver& operator=(const PortAudioRecorderDeviceDriver&) = delete;
    PortAudioRecorderDeviceDriver& operator=(PortAudioRecorderDeviceDriver&&) = delete;
    ~PortAudioRecorderDeviceDriver() override;

public: //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

public: //AudioRecorderDeviceBase(IAudioGrabberSound)
    bool startRecording() override;
    bool stopRecording() override;

 public: //Thread
    void threadRelease() override;
    bool threadInit() override;
    void run() override;

protected:
    void*   m_system_resource;
    int  m_device_id;
    void handleError();
};

#endif
