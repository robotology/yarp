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
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <portaudio.h>
#include <mutex>

#define DEFAULT_SAMPLE_RATE  (44100)
#define DEFAULT_NUM_CHANNELS    (2)
#define DEFAULT_DITHER_FLAG     (0)
#define DEFAULT_FRAMES_PER_BUFFER (512)
//#define DEFAULT_FRAMES_PER_BUFFER (1024)


class PortAudioRecorderDeviceDriverSettings
{
public:
    size_t cfg_rate = 0;
    size_t cfg_samples = 0;
    size_t cfg_recChannels = 0;
    int cfg_deviceNumber = 0;
};

class PortAudioRecorderDeviceDriver :
        public yarp::dev::IAudioGrabberSound,
        public yarp::dev::DeviceDriver,
        public yarp::os::Thread
{
private:
    PaStreamParameters  m_inputParameters;
    PaStream*           m_stream;
    PaError             m_err;
    yarp::dev::CircularAudioBuffer_16t*  m_recDataBuffer;
    PortAudioRecorderDeviceDriverSettings m_config;
    std::mutex     m_mutex;
    bool                m_isRecording;

public:
    PortAudioRecorderDeviceDriver();
    PortAudioRecorderDeviceDriver(const PortAudioRecorderDeviceDriver&) = delete;
    PortAudioRecorderDeviceDriver(PortAudioRecorderDeviceDriver&&) = delete;
    PortAudioRecorderDeviceDriver& operator=(const PortAudioRecorderDeviceDriver&) = delete;
    PortAudioRecorderDeviceDriver& operator=(PortAudioRecorderDeviceDriver&&) = delete;

    ~PortAudioRecorderDeviceDriver() override;

    bool open(yarp::os::Searchable& config) override;

    /**
     * Configures the device.
     *
     * rate: Sample rate to use, in Hertz.  Specify 0 to use a default.
     *
     * samples: Number of samples per call to getSound.  Specify
     * 0 to use a default.
     *
     * channels: Number of channels of input.  Specify
     * 0 to use a default.
     *
     * @return true on success
     */
    bool open(PortAudioRecorderDeviceDriverSettings& config);

    bool close(void) override;
    bool getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s) override;
    bool startRecording() override;
    bool stopRecording() override;

    bool getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    bool getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    bool resetRecordingAudioBuffer() override;

    void threadRelease() override;
    bool threadInit() override;
    void run() override;

protected:
    void*   m_system_resource;

    PortAudioRecorderDeviceDriverSettings m_driverConfig;
    void handleError(void);
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_portaudio portaudio

 A portable audio source, see yarp::dev::PortAudioRecorderDeviceDriver.
 Requires the PortAudio library (http://www.portaudio.com), at least v19.

*/


#endif
