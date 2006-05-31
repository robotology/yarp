// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARP2_FRAME_GRABBER__
#define __YARP2_FRAME_GRABBER__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Image.h>

/*! \file FrameGrabber.h define common interfaces for framer grabber devices */

namespace yarp{
    namespace dev {
        class IFrameGrabberRgb;
        class IFrameGrabberImage;
        class IFrameGrabber;
        class FrameGrabberOpenParameters;
    }
}

/** 
 * Common interface to a FrameGrabber.
 */
class yarp::dev::IFrameGrabber: public DeviceDriver
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
    virtual int height()=0;

    /** 
     * Return the width of each frame.
     * @return image width
     */
    virtual int width()=0;
};

/** 
 * RGB Interface to a FrameGrabber device.
 */
class yarp::dev::IFrameGrabberRgb:  public DeviceDriver
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
    virtual int height()=0;

    /** 
     * Return the width of each frame.
     * @return image width
     */
    virtual int width()=0;
};

class yarp::dev::IFrameGrabberImage: public DeviceDriver
{
     /**
     * Get an rgb image from the frame grabber, if required
     * demosaicking/color reconstruction is applied
     * 
     * @param image: the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) = 0;
    
    /** 
     * Return the height of each frame.
     * @return image height
     */
    virtual int height()=0;

    /** 
     * Return the width of each frame.
     * @return image width
     */
    virtual int width()=0;
};
#endif
