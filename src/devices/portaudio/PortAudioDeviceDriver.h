/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
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

#ifndef PortAudioDeviceDriverh
#define PortAudioDeviceDriverh

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Thread.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <portaudio.h>
#include "PortAudioBuffer.h"

#define DEFAULT_SAMPLE_RATE  (44100)
#define DEFAULT_NUM_CHANNELS    (2)
#define DEFAULT_DITHER_FLAG     (0)
#define DEFAULT_FRAMES_PER_BUFFER (512)
//#define DEFAULT_FRAMES_PER_BUFFER (1024)

namespace yarp {
    namespace dev {
        class PortAudioDeviceDriverSettings;
        class PortAudioDeviceDriver;
    }
}

class yarp::dev::PortAudioDeviceDriverSettings {
public:
    int rate;
    int samples;
    int channels;
    bool wantRead;
    bool wantWrite;
    int deviceNumber;
};

class streamThread : public yarp::os::Thread
{
   public:
   bool         something_to_play;
   bool         something_to_record;
   PaStream*    stream;
   void threadRelease() override;
   bool threadInit() override;
   void run() override;

   private:
   PaError      err;
   void handleError(void);
};

class yarp::dev::PortAudioDeviceDriver : public IAudioGrabberSound, 
            public IAudioRender, public DeviceDriver
{
private:
    PaStreamParameters  inputParameters;
    PaStreamParameters  outputParameters;
    PaStream*           stream;
    PaError             err;
    circularDataBuffers dataBuffers;
    int                 i;
    int                 numSamples;
    int                 numBytes;
    streamThread        pThread;

    PortAudioDeviceDriver(const PortAudioDeviceDriver&);
    void operator=(const PortAudioDeviceDriver&);

public:
    PortAudioDeviceDriver();

    virtual ~PortAudioDeviceDriver();

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
     * read: Should allow reading
     *
     * write: Should allow writing
     *
     * @return true on success
     */
    bool open(PortAudioDeviceDriverSettings& config);

    bool close(void) override;
    bool getSound(yarp::sig::Sound& sound) override;
    bool renderSound(yarp::sig::Sound& sound) override;
    bool startRecording() override;
    bool stopRecording() override;
    
    bool abortSound(void);
    bool immediateSound(yarp::sig::Sound& sound);
    bool appendSound(yarp::sig::Sound& sound);

protected:
    void *system_resource;
    int  numChannels;
    int  frequency;
    bool loopBack;

    PortAudioDeviceDriverSettings driverConfig;
    enum {RENDER_APPEND=0, RENDER_IMMEDIATE=1} renderMode;
    void handleError(void);
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_portaudio portaudio

 A portable audio source, see yarp::dev::PortAudioDeviceDriver.
 Requires the PortAudio library (http://www.portaudio.com), at least v19.

*/


#endif
