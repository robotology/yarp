/*
 * Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT)
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
#include <portaudio.h>
#include "PortAudioRecorderBuffer.h"

#define DEFAULT_SAMPLE_RATE  (44100)
#define DEFAULT_NUM_CHANNELS    (2)
#define DEFAULT_DITHER_FLAG     (0)
#define DEFAULT_FRAMES_PER_BUFFER (512)
//#define DEFAULT_FRAMES_PER_BUFFER (1024)

namespace yarp {
    namespace dev {
        class PortAudioRecorderDeviceDriverSettings;
        class PortAudioRecorderDeviceDriver;
    }
}

class yarp::dev::PortAudioRecorderDeviceDriverSettings {
public:
    size_t cfg_rate;
    size_t cfg_samples;
    size_t cfg_recChannels;
    int cfg_deviceNumber;
    PortAudioRecorderDeviceDriverSettings()
    {
        cfg_rate = 0;
        cfg_samples = 0;
        cfg_recChannels = 0;
        cfg_deviceNumber = 0;
    }
};

class recStreamThread : public yarp::os::Thread
{
   public:
   bool         something_to_record;
   PaStream*    stream;
   void threadRelease() override;
   bool threadInit() override;
   void run() override;

   private:
   PaError      err;
   void handleError(void);
};

class yarp::dev::PortAudioRecorderDeviceDriver : public IAudioGrabberSound, 
                                         public DeviceDriver
{
private:
    PaStreamParameters  m_inputParameters;
    PaStream*           m_stream;
    PaError             m_err;
    circularDataBuffers m_dataBuffers;
    PortAudioRecorderDeviceDriverSettings m_config;
    recStreamThread     m_pThread;

    PortAudioRecorderDeviceDriver(const PortAudioRecorderDeviceDriver&);

public:
    PortAudioRecorderDeviceDriver();

    virtual ~PortAudioRecorderDeviceDriver();

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
