// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __PortAudioDeviceDriverh__
#define __PortAudioDeviceDriverh__

#include <yarp/os/ManagedBytes.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>

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
};

/**
 * @ingroup dev_impl
 *
 * A basic microphone input source based on the PortAudio library.
 * The implementation is very simple right now - if you want to
 * use this for real you should fix it up.
 *
 */
class yarp::dev::PortAudioDeviceDriver : public IAudioGrabberSound, 
            public IAudioRender, public DeviceDriver
{
private:
	PortAudioDeviceDriver(const PortAudioDeviceDriver&);
	void operator=(const PortAudioDeviceDriver&);

public:
	PortAudioDeviceDriver();

	virtual ~PortAudioDeviceDriver();

    virtual bool open(yarp::os::Searchable& config);

    /**
     * Configures the device.
     * @param rate Sample rate to use, in Hertz.  Specify 0 to use a default.
     * @param samples Number of samples per call to getSound.  Specify
     * 0 to use a default.
     * @param channels Number of channels of input.  Specify
     * 0 to use a default.
     * @param wantRead Should allow reading
     * @param wantWrite Should allow writing
     * @return true on success
     */
    bool open(PortAudioDeviceDriverSettings& config);

	virtual bool close(void);

    virtual bool getSound(yarp::sig::Sound& sound);

    virtual bool renderSound(yarp::sig::Sound& sound);

protected:
    void *system_resource;
    yarp::os::ManagedBytes buffer;
    int num_samples;
    int num_channels;
    int set_freq;
    bool canRead, canWrite, loopBack;

    bool delayed;
    PortAudioDeviceDriverSettings delayedConfig;

    void checkDelay(yarp::sig::Sound& sound);
};


/*
  class yarp::dev::PortAudioRender : public IAudioRender, public DeviceDriver {
  public:
  PortAudioRender();

  virtual ~PortAudioRender();

  virtual bool open(yarp::os::Searchable& config);

  virtual bool close();

  virtual bool renderSound(yarp::sig::Sound& sound);
  };
*/

/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_portaudio portaudio

 A portable audio source, see yarp::dev::PortAudioDeviceDriver.
 Requires the PortAudio library (http://www.portaudio.com), at least v19.

*/


#endif
