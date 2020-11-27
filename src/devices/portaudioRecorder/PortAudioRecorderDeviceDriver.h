/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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

#define DEFAULT_SAMPLE_RATE  (44100)
#define DEFAULT_NUM_CHANNELS    (2)
#define DEFAULT_DITHER_FLAG     (0)
#define DEFAULT_FRAMES_PER_BUFFER (512)
//#define DEFAULT_FRAMES_PER_BUFFER (1024)


/**
 * @ingroup dev_impl_media
 *
 * \brief `portaudioRecorder`: A portable audio source, see yarp::dev::PortAudioRecorderDeviceDriver.
 *
 * Requires the PortAudio library (http://www.portaudio.com), at least v19.
 * Documentation to be added
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

    public:
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    public:
    bool startRecording() override;
    bool stopRecording() override;

    public:
    void threadRelease() override;
    bool threadInit() override;
    void run() override;

protected:
    void*   m_system_resource;
    int  m_device_id;
    void handleError();
};

#endif
