// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
    }
}

/** 
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
 * Control interface for frame grabber devices.
 */
class yarp::dev::IFrameGrabberControls
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameGrabberControls(){}

    /**
     * Set the brightness.
     * @param v new value for parameter.
     * @return true on success.
     */
    virtual bool setBrightness(double v) = 0;

    /**
     * Set the shutter parameter.
     * @param v new value for parameter.
     * @return true on success.
     */
    virtual bool setShutter(double v) = 0;

    /**
     * Set the gain.
     * @param v new value for parameter.
     * @return true on success.
     */
    virtual bool setGain(double v) = 0;

    /**
     * Read the brightness parameter.
     * @return the current brightness value.
     */
    virtual double getBrightness() const = 0;

    /**
     * Read the shutter parameter.
     * @return the current shutter value.
     */
    virtual double getShutter() const = 0;

    /**
     * Read the gain parameter.
     * @return the current gain value.
     */
    virtual double getGain() const = 0;
};
#endif
