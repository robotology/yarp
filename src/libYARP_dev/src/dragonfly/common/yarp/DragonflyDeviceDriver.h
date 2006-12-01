// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

///
/// $Id: DragonflyDeviceDriver.h,v 1.25 2006-12-01 11:24:48 eshuy Exp $
///
///

#ifndef __DragonflyDeviceDriverh__
#define __DragonflyDeviceDriverh__

//=============================================================================
// YARP Includes
//=============================================================================

// May 06, readapted for YARP2 by nat

#include <yarp/os/Semaphore.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/FrameGrabberInterfaces.h>

namespace yarp {
    namespace dev {
        class DragonflyOpenParameters;
        class DragonflyDeviceDriver;
    }
}

/**
 * \file DragonflyDeviceDriver.h device driver for managing the 
 * Dragonfly IEEE-1394 Camera (WINNT version)
 */

/**
 * Structure for defining the open() parameters of the camera.
 */
class yarp::dev::DragonflyOpenParameters
{
public:
	// Parameters
	unsigned int _unit_number;
	unsigned int _size_x;
	unsigned int _size_y;
	unsigned int _video_type;

	int _whiteR;
	int _whiteB;

	int _brightness;
	int _exposure;
	int _shutter;
	int _gain;

	/**
	 * Constructor. Add here the parameters for the open().
	 */
	DragonflyOpenParameters()
	{
		// parameters initialization
		_unit_number = 0;
		_size_x = 640;
		_size_y = 480;
		_video_type = 0;

		//reasonable default values
		_brightness=0;
		_exposure=300;
		_shutter=320;
		_gain=500;
		_whiteR=20;
		_whiteB=50;
	}

};

/**
 * @ingroup dev_impl_media
 *
 * A Point Grey Dragonfly digital camera (or, on Linux, any digital camera).
 */
class yarp::dev::DragonflyDeviceDriver : 
    public IFrameGrabber, public IFrameGrabberRgb, public IFrameGrabberImage, public IFrameGrabberControls, public DeviceDriver
{
private:
	DragonflyDeviceDriver(const DragonflyDeviceDriver&);
	void operator=(const DragonflyDeviceDriver&);

public:
	/**
	 * Constructor.
	 */
	DragonflyDeviceDriver();

	/**
	 * Destructor.
	 */
	virtual ~DragonflyDeviceDriver();

    // temp: here for debug purposes only
    void recColorFSBilinear(const unsigned char *src, unsigned char *out);
    void recColorFSNN(const unsigned char *src, unsigned char *out);
    void recColorHSBilinear(const unsigned char *src, unsigned char *out);


    /**
	 * Open the device driver.
     * @param par parameters for the device driver
	 * @return returns true on success, false on failure.
	 */
    bool open(const DragonflyOpenParameters& par);

    virtual bool open(yarp::os::Searchable& config)
	{
        DragonflyOpenParameters params;
		yarp::os::Value *value;
		if (config.check("unit_number",value)||config.check("d",value)) {
			params._unit_number = value->asInt();
		}
		if (config.check("size_x",value)||config.check("width",value)){
			params._size_x  = value->asInt();
		}
		if (config.check("size_y",value)||config.check("height",value)){
			params._size_y  = value->asInt();
		}
		params._video_type = config.find("video_type").asInt();

		//params._offset_y = config.find("offset_y").asInt();
		//params._offset_x = config.find("offset_x").asInt();
		//params._alfa = (float)config.find("alfa").asInt();
		yarp::os::Bottle& whites = config.findGroup("white_balance");
		if (!whites.isNull()) {
			params._whiteR = whites.get(1).asInt();
			params._whiteB = whites.get(2).asInt();
		}
	
		if (config.check("brightness", value)){
			params._brightness=value->asInt();
		}
		if (config.check("exposure", value)){
			params._exposure=value->asInt();
		}
		if (config.check("shutter", value)){
			params._shutter=value->asInt();
		}
		if (config.check("gain", value)){
			params._gain=value->asInt();
		}

		return open(params);
    }

	/**
	 * Closes the device driver.
	 * @return returns true/false on success/failure.
	 */
	virtual bool close(void);

    /**
     * Implements FrameGrabber basic interface.
     * @param buffer the pointer to the array to store the last frame.
     * @return returns true/false on success/failure.
     */
    virtual bool getRawBuffer(unsigned char *buffer);

    /**
     * Implements the Frame grabber basic interface.
     * @return the size of the raw buffer (for the Dragonfly
     * camera this is 1x640x480).
     */
    virtual int getRawBufferSize();

    /**
     * Implements FrameGrabber basic interface.
     */
    virtual int height() const;
    
    /**
     * Implements FrameGrabber basic interface.
     */
    virtual int width() const;

    /** 
     * FrameGrabber bgr interface, returns the last acquired frame as
     * a buffer of bgr triplets. A demosaicking method is applied to 
     * reconstuct the color from the Bayer pattern of the sensor.
     * @param buffer pointer to the array that will contain the last frame.
     * @return true/false upon success/failure
     */
    virtual bool getRgbBuffer(unsigned char *buffer);

    /** 
     * FrameGrabber image interface, returns the last acquired frame as
     * an rgb image. A demosaicking method is applied to 
     * reconstuct the color from the Bayer pattern of the sensor.
     * @param image that will store the last frame.
     * @return true/false upon success/failure
     */
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    /**
     * Set image brightness.
     */
    virtual bool setBrightness(double v);

    /**
     * Set shutter time.
     */
    virtual bool setShutter(double v);

    /**
     * Set gain.
     */
    virtual bool setGain(double v);

    /**
     * Get shutter time.
     */
    virtual double getShutter() const;

    /**
     * Get gain.
     */
    virtual double getGain() const;

    /**
     * Get image brightness.
     */
    virtual double getBrightness() const;

	virtual bool setWhiteBalance(double r, double b);

protected:
	void *system_resources;
};

/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_dragonfly dragonfly

 A streaming digital camera source, see yarp::dev::DragonflyDeviceDriver.

*/


#endif
