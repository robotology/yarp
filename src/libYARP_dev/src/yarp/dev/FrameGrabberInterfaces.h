/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_FRAMEGRABBERINTERFACES_H
#define YARP_FRAMEGRABBERINTERFACES_H

#include <string>

#include <yarp/dev/api.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>

#include <yarp/dev/IFrameGrabber.h>
#include <yarp/dev/IFrameGrabberRgb.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameWriterImage.h>

/*! \file FrameGrabberInterfaces.h define common interfaces for frame
  grabber devices */

typedef enum {
    YARP_CROP_RECT = 0,             // Rectangular region of interest style, requires the two corner as a parameter
    YARP_CROP_LIST                  // Unordered list of points, the returned image will be a nx1 image with n the
                                    // number of points required by user (size of input vector), with the corresponding
                                    // pixel color.
} cropType_id_t;


namespace yarp {
namespace dev {

/**
 * @ingroup dev_iface_media
 *
 * Read a YARP-format image from a device.
 */
class YARP_dev_API IFrameGrabberImage
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
     * Get a crop of the rgb image from the frame grabber, if required
     * demosaicking/color reconstruction is applied
     *
     * Note: this is not configuring the camera sensor to acquire a crop
     *       of the image, nor to generate a cropped version of the streaming.
     *       Instead, the full image is acquired and then a crop is created from
     *       it. The crop is meant to be created by the image producer upon user
     *       request via RPC call.
     *
     * @param cropType enum specifying how the crop shall be generated, defined at FrameGrabberInterfaces.h
     * @param vertices the input coordinate (u,v) required by the cropType
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImageCrop(cropType_id_t cropType, yarp::sig::VectorOf<std::pair<int, int> > vertices, yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) { return false; };

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
class YARP_dev_API IFrameGrabberImageRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameGrabberImageRaw(){}
    /**
     * Get a raw image from the frame grabber
     *
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) = 0;

    /**
     * Get a crop of the rgb image from the frame grabber, if required
     * demosaicking/color reconstruction is applied
     *
     * Note: this is not configuring the camera sensor to acquire a crop
     *       of the image, nor to generate a cropped version of the streaming.
     *       Instead, the full image is acquired and then a crop is created from
     *       it. The crop is meant to be created by the image producer upon user
     *       request via RPC call.
     *
     * @param cropType enum specifying how the crop shall be generated, defined at FrameGrabberInterfaces.h
     * @param vertices the input coordinate (u,v) required by the cropType
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImageCrop(cropType_id_t cropType, yarp::sig::VectorOf<std::pair<int, int> > vertices, yarp::sig::ImageOf<yarp::sig::PixelMono>& image) { return false; };

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

class YARP_dev_API IFrameGrabberControlsDC1394
{
public:
    virtual ~IFrameGrabberControlsDC1394() {}

    // 12 13 14
    virtual unsigned int getVideoModeMaskDC1394()=0;//{ return 0xFFFFFFFF; }
    virtual unsigned int getVideoModeDC1394()=0;//{ return 0; }
    virtual bool setVideoModeDC1394(int video_mode)=0;//{ return true; }

    // 15 16 17
    virtual unsigned int getFPSMaskDC1394()=0;//{ return 0xFFFFFFFF; }
    virtual unsigned int getFPSDC1394()=0;//{ return 0; }
    virtual bool setFPSDC1394(int fps)=0;//{ return true; }

    // 18 19
    virtual unsigned int getISOSpeedDC1394()=0;//{ return 0; }
    virtual bool setISOSpeedDC1394(int speed)=0;//{ return true; }

    // 20 21 22
    virtual unsigned int getColorCodingMaskDC1394(unsigned int video_mode)=0;//{ return 0xFFFFFFFF; }
    virtual unsigned int getColorCodingDC1394()=0;//{ return 0; }
    virtual bool setColorCodingDC1394(int coding)=0;//{ return true; }
    /*{
        b=r=0.5;
        return true;
    }*/

    // 25 26 27
    virtual bool getFormat7MaxWindowDC1394(unsigned int &xdim,unsigned int &ydim,unsigned int &xstep,unsigned int &ystep,unsigned int &xoffstep,unsigned int &yoffstep)=0;
    /*{
        xdim=324; ydim=244; xstep=2; ystep=2;
        return true;
    }*/
    virtual bool getFormat7WindowDC1394(unsigned int &xdim,unsigned int &ydim,int &x0,int &y0)=0;
    /*{
        xdim=324; ydim=244;
        return true;
    }*/
    virtual bool setFormat7WindowDC1394(unsigned int xdim,unsigned int ydim,int x0,int y0)=0;//{ return true; }

    // 28
    virtual bool setOperationModeDC1394(bool b1394b)=0;//{ return true; }
    // 29
    virtual bool getOperationModeDC1394()=0;//{ return true; }
    // 30
    virtual bool setTransmissionDC1394(bool bTxON)=0;//{ return true; }
    // 31
    virtual bool getTransmissionDC1394()=0;//{ return true; }
    // 32
    //virtual bool setBayerDC1394(bool bON)=0;//{ return true; }
    // 33
    //virtual bool getBayerDC1394()=0;//{ return true; }

    // 34 35 36 37
    virtual bool setBroadcastDC1394(bool onoff)=0;//{ return true; }
    virtual bool setDefaultsDC1394()=0;//{ return true; }
    virtual bool setResetDC1394()=0;//{ return true; }
    virtual bool setPowerDC1394(bool onoff)=0;//{ return true; }

    // 38
    virtual bool setCaptureDC1394(bool bON)=0;//{ return true; }

    // 39
    virtual unsigned int getBytesPerPacketDC1394()=0;//{ return 0; }

    // 40
    virtual bool setBytesPerPacketDC1394(unsigned int bpp)=0;//{ return true; }
};

} // namespace dev
} // namespace yarp
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
#include <yarp/dev/FrameGrabberControl2.h>
#endif

#endif // YARP_FRAMEGRABBERINTERFACES_H
