// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __MicrophoneDeviceDriverh__
#define __MicrophoneDeviceDriverh__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>

namespace yarp {
    namespace dev {
        class MicrophoneDeviceDriver;
    }
}

/**
 * @ingroup dev_impl_media
 *
 * A basic microphone input source.
 * The implementation is very simple right now - if you want to
 * use this for real you should fix it up.
 *
 */
class yarp::dev::MicrophoneDeviceDriver : 
    public IAudioGrabberSound, public DeviceDriver
{
private:
	MicrophoneDeviceDriver(const MicrophoneDeviceDriver&);
	void operator=(const MicrophoneDeviceDriver&);

public:
	/**
	 * Constructor.
	 */
	MicrophoneDeviceDriver();

	/**
	 * Destructor.
	 */
	virtual ~MicrophoneDeviceDriver();

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

    virtual bool startRecording();
    virtual bool stopRecording();

protected:
	void *system_resources;
    int dsp;
};


#endif
