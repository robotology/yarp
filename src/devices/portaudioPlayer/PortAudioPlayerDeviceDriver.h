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

#ifndef PortAudioPlayerDeviceDriverh
#define PortAudioPlayerDeviceDriverh

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Thread.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <portaudio.h>

#define DEFAULT_SAMPLE_RATE  (44100)
#define DEFAULT_NUM_CHANNELS    (2)
#define DEFAULT_DITHER_FLAG     (0)
#define DEFAULT_FRAMES_PER_BUFFER (512)
//#define DEFAULT_FRAMES_PER_BUFFER (1024)

namespace yarp {
    namespace dev {
        class PortAudioPlayerDeviceDriverSettings;
        class PortAudioPlayerDeviceDriver;
    }
}

class yarp::dev::PortAudioPlayerDeviceDriverSettings {
public:
    size_t cfg_rate;
    size_t cfg_samples;
    size_t cfg_playChannels;
    int cfg_deviceNumber;
    PortAudioPlayerDeviceDriverSettings()
    {
        cfg_rate = 0;
        cfg_samples = 0;
        cfg_playChannels = 0;
        cfg_deviceNumber = 0;
    }
};

class playStreamThread : public yarp::os::Thread
{
   public:
   bool         something_to_play;
   PaStream*    stream;
   void threadRelease() override;
   bool threadInit() override;
   void run() override;

   private:
   PaError      err;
   void handleError(void);
};

class yarp::dev::PortAudioPlayerDeviceDriver : public IAudioRender,
                                               public DeviceDriver
{
private:
    PaStreamParameters  m_outputParameters;
    PaStream*           m_stream;
    PaError             m_err;
    yarp::dev::CircularAudioBuffer_16t* m_playDataBuffer;
    PortAudioPlayerDeviceDriverSettings m_config;
    playStreamThread    m_pThread;
    yarp::os::Mutex     m_mutex;

    PortAudioPlayerDeviceDriver(const PortAudioPlayerDeviceDriver&);

public:
    PortAudioPlayerDeviceDriver();

    virtual ~PortAudioPlayerDeviceDriver();

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
    bool open(PortAudioPlayerDeviceDriverSettings& config);

    bool close(void) override;
    bool renderSound(const yarp::sig::Sound& sound) override;
    bool startPlayback() override;
    bool stopPlayback() override;
    
    bool abortSound(void);
    bool immediateSound(const yarp::sig::Sound& sound);
    bool appendSound(const yarp::sig::Sound& sound);

    bool getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) override;
    bool getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) override;
    bool resetPlaybackAudioBuffer() override;

protected:
    void*   m_system_resource;

    PortAudioPlayerDeviceDriverSettings m_driverConfig;
    enum {RENDER_APPEND=0, RENDER_IMMEDIATE=1} renderMode;
    void handleError(void);
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_portaudio portaudio

 A portable audio source, see yarp::dev::PortAudioPlayerDeviceDriver.
 Requires the PortAudio library (http://www.portaudio.com), at least v19.

*/


#endif
