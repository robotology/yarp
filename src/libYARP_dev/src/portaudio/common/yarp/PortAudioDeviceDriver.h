// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __PortAudioDeviceDriverh__
#define __PortAudioDeviceDriverh__

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
class yarp::dev::PortAudioDeviceDriver : 
    public IAudioGrabberSound, public DeviceDriver
{
private:
	PortAudioDeviceDriver(const PortAudioDeviceDriver&);
	void operator=(const PortAudioDeviceDriver&);

public:
	/**
	 * Constructor.
	 */
	PortAudioDeviceDriver();

	/**
	 * Destructor.
	 */
	virtual ~PortAudioDeviceDriver();

    /**
	 * Open the device driver.
     * @param config parameters for the device driver
	 * @return returns true on success, false on failure.
	 */
    virtual bool open(yarp::os::Searchable& config);

	/**
	 * Closes the device driver.
	 * @return returns true/false on success/failure.
	 */
	virtual bool close(void);

    virtual bool getSound(yarp::sig::Sound& sound);

protected:
	void *system_resource;
    int dsp;
};

/**
  * @ingroup dev_runtime
  * \defgroup cmd_device_portaudio portaudio

A portable audio source, see yarp::dev::PortAudioDeviceDriver.

*/


#endif
