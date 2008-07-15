// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick and Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARP2_FRAME_GRABBER__
#define __YARP2_FRAME_GRABBER__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Image.h>

/*! \file FrameGrabberInterfaces.h define common interfaces for frame
  grabber devices */

namespace yarp{
    namespace dev {
        class IFrameGrabberRgb;
        class IFrameGrabberImage;
        class IFrameGrabber;
        class IFrameGrabberControls;
        class FrameGrabberOpenParameters;
        class IFrameWriterImage;
    }
}



/** 
 * @ingroup dev_iface_media
 *
 * Common interface to a FrameGrabber.
 */
class yarp::dev::IFrameGrabber
{
public:
    virtual ~IFrameGrabber(){}
    
    /**
     * Get the raw buffer from the frame grabber. The driver returns 
     * a copy of the internal memory buffer acquired by the frame grabber, no
     * post processing is applied (e.g. no color reconstruction/demosaicking).
     * The user must allocate the buffer; the size of the buffer, in bytes, 
     * is determined by calling getRawBufferSize().
     * @param buffer: pointer to the buffer to be filled (must be previously allocated)
     * @return true/false upon success/failure
     */
    virtual bool getRawBuffer(unsigned char *buffer)=0;

    /**
     * Get the size of the card's internal buffer, the user should use this 
     * method to allocate the storage to contain a raw frame (getRawBuffer).
     * @return the size of the internal buffer, in bytes.
     **/
    virtual int getRawBufferSize()=0;

    /** 
     * Return the height of each frame.
     * @return image height
     */
    virtual int height() const =0;

    /** 
     * Return the width of each frame.
     * @return image width
     */
    virtual int width() const =0;
};

/** 
 * @ingroup dev_iface_media
 *
 * RGB Interface to a FrameGrabber device.
 */
class yarp::dev::IFrameGrabberRgb
{
public:
    virtual ~IFrameGrabberRgb(){}
    /**
     * Get a rgb buffer from the frame grabber, if required
     * demosaicking/color reconstruction is applied
     * 
     * @param buffer: pointer to the buffer to be filled (must be previously allocated)
     * @return true/false upon success/failure
     */
    virtual bool getRgbBuffer(unsigned char *buffer)=0;

    /** 
     * Return the height of each frame.
     * @return image height
     */
    virtual int height() const =0;

    /** 
     * Return the width of each frame.
     * @return image width
     */
    virtual int width() const =0;
};

/**
 * @ingroup dev_iface_media
 *
 * Read a YARP-format image from a device.
 */
class yarp::dev::IFrameGrabberImage
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameGrabberImage(){}
    /**
     * Get an rgb image from the frame grabber, if required
     * demosaicking/color reconstruction is applied
     * 
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) = 0;
    
    /** 
     * Return the height of each frame.
     * @return image height
     */
    virtual int height() const =0;

    /** 
     * Return the width of each frame.
     * @return image width
     */
    virtual int width() const =0;
};

/**
 * @ingroup dev_iface_media
 *
 * Read a YARP-format image to a device.
 */
class yarp::dev::IFrameWriterImage
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameWriterImage(){}

    /**
     * Write an image to the device.
     * 
     * @param image the image to write
     * @return true/false upon success/failure
     */
    virtual bool putImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) = 0;
};

/**
 * @ingroup dev_iface_media
 *
 * Control interface for frame grabber devices.
 */
class yarp::dev::IFrameGrabberControls
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameGrabberControls(){}

// set
    /**
     * Set the brightness.
     * @param v new value for parameter.
     * @return true on success.
     */
	virtual bool setBrightness(double v)=0;
    /**
     * Set the exposure.
     * @param v new value for parameter.
     * @return true on success.
     */
    virtual bool setExposure(double v)=0;
    /**
     * Set the sharpness.
     * @param v new value for parameter.
     * @return true on success.
     */
	virtual bool setSharpness(double v)=0;
	/**
     * Set the white balance for the frame grabber.
     * @param blue component gain.
     * @param red component gain.
     * @return true/false if successful or not.
     */
	virtual bool setWhiteBalance(double blue, double red)=0;
    /**
     * Set the hue.
     * @param v new value for parameter.
     * @return true on success.
     */
	virtual bool setHue(double v)=0;
    /**
     * Set the saturation.
     * @param v new value for parameter.
     * @return true on success.
     */
	virtual bool setSaturation(double v)=0;
    /**
     * Set the gamma.
     * @param v new value for parameter.
     * @return true on success.
     */
	virtual bool setGamma(double v)=0;
    /**
     * Set the shutter parameter.
     * @param v new value for parameter.
     * @return true on success.
     */
	virtual bool setShutter(double v)=0;
    /**
     * Set the gain.
     * @param v new value for parameter.
     * @return true on success.
     */
    virtual bool setGain(double v)=0;
    /**
     * Set the iris.
     * @param v new value for parameter.
     * @return true on success.
     */
    virtual bool setIris(double v)=0;

    // not implemented
    //virtual bool setTemperature(double v)=0;
    //virtual bool setWhiteShading(double r,double g,double b)=0;
    //virtual bool setOpticalFilter(double v)=0;
    //virtual bool setCaptureQuality(double v)=0;
    
// get
    /**
     * Read the brightness parameter.
     * @return the current brightness value.
     */
	virtual double getBrightness() const=0;
    /**
     * Read the exposure parameter.
     * @return the current exposure value.
     */
	virtual double getExposure() const=0;
    /**
     * Read the sharpness parameter.
     * @return the current sharpness value.
     */	
	virtual double getSharpness() const=0;
	/**
     * Read the white balance parameters.
     * @param blue reference to return value for the red parameter.
	 * @param red reference to return value for the green parameter.
	 * @return true/false.
     */
    virtual bool getWhiteBalance(double &blue, double &red) const=0;
    /**
     * Read the hue parameter.
     * @return the current hue value.
     */	
	virtual double getHue() const=0;
    /**
     * Read the saturation parameter.
     * @return the current saturation value.
     */	
	virtual double getSaturation() const=0;
    /**
     * Read the gamma parameter.
     * @return the current gamma value.
     */	
	virtual double getGamma() const=0;
    /**
     * Read the shutter parameter.
     * @return the current shutter value.
     */
    virtual double getShutter() const=0;
    /**
     * Read the gain parameter.
     * @return the current gain value.
     */
    virtual double getGain() const=0;
    /**
     * Read the iris parameter.
     * @return the current iris value.
     */
    virtual double getIris() const=0;
    
    // not implemented
    //virtual double getTemperature() const=0;
    //virtual bool getWhiteShading(double &r,double &g,double &b) const=0;
    //virtual double getOpticalFilter() const=0;
    //virtual double getCaptureQuality() const=0;
};
#endif
