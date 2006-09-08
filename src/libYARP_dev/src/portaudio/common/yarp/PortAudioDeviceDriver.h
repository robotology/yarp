// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __PortAudioDeviceDriverh__
#define __PortAudioDeviceDriverh__

#include <yarp/os/ManagedBytes.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>

namespace yarp {
    namespace dev {
        class PortAudioDeviceDriver;
    }
}


/**
 * @ingroup dev_impl
 *
 * A basic microphone input source based on the PortAudio library.
 * The implementation is very simple right now - if you want to
 * use this for real you should fix it up.
 *
 */
class yarp::dev::PortAudioDeviceDriver : public IAudioGrabberSound, 
            public DeviceDriver
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
     * @return true on success
     */
    bool open(int rate, int samples, int channels);

	virtual bool close(void);

    virtual bool getSound(yarp::sig::Sound& sound);

protected:
    void *system_resource;
    yarp::os::ManagedBytes buffer;
    int num_samples;
    int num_channels;
};

/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_portaudio portaudio

 A portable audio source, see yarp::dev::PortAudioDeviceDriver.
 Requires the PortAudio library (http://www.portaudio.com), at least v19.

*/


#endif
