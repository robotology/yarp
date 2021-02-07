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
 * \brief `portaudioPlayer`: Documentation to be added
 *
 * Requires the PortAudio library (http://www.portaudio.com), at least v19.
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
    bool configureDeviceAndStart();

public: //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

public: //AudioRecorderDeviceBase(IAudioGrabberSound)
    void waitUntilPlaybackStreamIsComplete() override;
    bool setHWGain(double gain) override;
    bool interruptDeviceAndClose() override;

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
