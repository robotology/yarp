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

/*! \file FrameGrabberInterfaces.h define common interfaces for frame
  grabber devices */

typedef enum {
    BUS_UNKNOWN = 0,
    BUS_FIREWIRE,
    BUS_USB
} BusType;

typedef enum {
    MODE_UNKNOWN = 0,
    MODE_MANUAL,
    MODE_AUTO
} FeatureMode;

typedef struct {
    BusType busType;
    std::string deviceDescription;
} CameraDescriptor;

typedef enum {
    YARP_CROP_RECT = 0,             // Rectangular region of interest style, requires the two corner as a parameter
    YARP_CROP_LIST                  // Unordered list of points, the returned image will be a nx1 image with n the
                                    // number of points required by user (size of input vector), with the corresponding
                                    // pixel color.
} cropType_id_t;

typedef enum {
    YARP_FEATURE_INVALID=-1,
    YARP_FEATURE_BRIGHTNESS=0,
    YARP_FEATURE_EXPOSURE,
    YARP_FEATURE_SHARPNESS,
    YARP_FEATURE_WHITE_BALANCE,
    YARP_FEATURE_HUE,
    YARP_FEATURE_SATURATION,
    YARP_FEATURE_GAMMA,
    YARP_FEATURE_SHUTTER,
    YARP_FEATURE_GAIN,
    YARP_FEATURE_IRIS,
    YARP_FEATURE_FOCUS,
    YARP_FEATURE_TEMPERATURE,
    YARP_FEATURE_TRIGGER,
    YARP_FEATURE_TRIGGER_DELAY,
    YARP_FEATURE_WHITE_SHADING,
    YARP_FEATURE_FRAME_RATE,
    YARP_FEATURE_ZOOM,
    YARP_FEATURE_PAN,
    YARP_FEATURE_TILT,
    YARP_FEATURE_OPTICAL_FILTER,
    YARP_FEATURE_CAPTURE_SIZE,
    YARP_FEATURE_CAPTURE_QUALITY,
    YARP_FEATURE_MIRROR,
    YARP_FEATURE_NUMBER_OF          // this has to be the last one
} cameraFeature_id_t;


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

/**
 * @ingroup dev_iface_media
 *
 * Read a YARP-format image to a device.
 */
class YARP_dev_API IFrameWriterImage
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
class YARP_dev_API IFrameGrabberControls
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameGrabberControls(){}
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
// set
    /**
     * Set the brightness.
     * @param v new value for parameter.
     * @return true on success.
     */
    YARP_DEPRECATED_MSG("Use setFeature(YARP_FEATURE_BRIGHTNESS, v) instead")
    virtual bool setBrightness(double v) { return false; }
    /**
     * Set the exposure.
     * @param v new value for parameter.
     * @return true on success.
     */
    YARP_DEPRECATED_MSG("Use setFeature(YARP_FEATURE_EXPOSURE, v) instead")
    virtual bool setExposure(double v) { return false; }
    /**
     * Set the sharpness.
     * @param v new value for parameter.
     * @return true on success.
     */
    YARP_DEPRECATED_MSG("Use setFeature(YARP_FEATURE_SHARPNESS, v) instead")
    virtual bool setSharpness(double v) { return false; }
    /**
     * Set the white balance for the frame grabber.
     * @param blue component gain.
     * @param red component gain.
     * @return true/false if successful or not.
     */
    YARP_DEPRECATED_MSG("Use setFeature(YARP_FEATURE_WHITE_BALANCE, blue, red) instead")
    virtual bool setWhiteBalance(double blue, double red) { return false; }
    /**
     * Set the hue.
     * @param v new value for parameter.
     * @return true on success.
     */
    YARP_DEPRECATED_MSG("Use setFeature(YARP_FEATURE_HUE, v) instead")
    virtual bool setHue(double v) { return false; }
    /**
     * Set the saturation.
     * @param v new value for parameter.
     * @return true on success.
     */
    YARP_DEPRECATED_MSG("Use setFeature(YARP_FEATURE_SATURATION, v) instead")
    virtual bool setSaturation(double v) { return false; }
    /**
     * Set the gamma.
     * @param v new value for parameter.
     * @return true on success.
     */
    YARP_DEPRECATED_MSG("Use setFeature(YARP_FEATURE_GAMMA, v) instead")
    virtual bool setGamma(double v) { return false; }
    /**
     * Set the shutter parameter.
     * @param v new value for parameter.
     * @return true on success.
     */
    YARP_DEPRECATED_MSG("Use setFeature(YARP_FEATURE_SHUTTER, v) instead")
    virtual bool setShutter(double v) { return false; }
    /**
     * Set the gain.
     * @param v new value for parameter.
     * @return true on success.
     */
    YARP_DEPRECATED_MSG("Use setFeature(YARP_FEATURE_GAIN, v) instead")
    virtual bool setGain(double v) { return false; }
    /**
     * Set the iris.
     * @param v new value for parameter.
     * @return true on success.
     */
    YARP_DEPRECATED_MSG("Use setFeature(YARP_FEATURE_IRIS, v) instead")
    virtual bool setIris(double v) { return false; }

// get
    /**
     * Read the brightness parameter.
     * @return the current brightness value.
     */
    YARP_DEPRECATED_MSG("Use getFeature(YARP_FEATURE_BRIGHTNESS, v) instead")
    virtual double getBrightness() { return -1.0; }
    /**
     * Read the exposure parameter.
     * @return the current exposure value.
     */
    YARP_DEPRECATED_MSG("Use getFeature(YARP_FEATURE_EXPOSURE, v) instead")
    virtual double getExposure() { return -1.0; }
    /**
     * Read the sharpness parameter.
     * @return the current sharpness value.
     */
    YARP_DEPRECATED_MSG("Use getFeature(YARP_FEATURE_SHARPNESS, v) instead")
    virtual double getSharpness() { return -1.0; }
    /**
     * Read the white balance parameters.
     * @param blue reference to return value for the red parameter.
     * @param red reference to return value for the green parameter.
     * @return true/false.
     */
    YARP_DEPRECATED_MSG("Use getFeature(YARP_FEATURE_WHITE_BALANCE, blue, red) instead")
    virtual bool getWhiteBalance(double &blue, double &red) { return false; }
    /**
     * Read the hue parameter.
     * @return the current hue value.
     */
    YARP_DEPRECATED_MSG("Use getFeature(YARP_FEATURE_HUE, v) instead")
    virtual double getHue() { return -1.0; }
    /**
     * Read the saturation parameter.
     * @return the current saturation value.
     */
    YARP_DEPRECATED_MSG("Use getFeature(YARP_FEATURE_SATURATION, v) instead")
    virtual double getSaturation() { return -1.0; }
    /**
     * Read the gamma parameter.
     * @return the current gamma value.
     */
    YARP_DEPRECATED_MSG("Use getFeature(YARP_FEATURE_GAMMA, v) instead")
    virtual double getGamma() { return -1.0; }
    /**
     * Read the shutter parameter.
     * @return the current shutter value.
     */
    YARP_DEPRECATED_MSG("Use getFeature(YARP_FEATURE_SHUTTER, v) instead")
    virtual double getShutter() { return -1.0; }
    /**
     * Read the gain parameter.
     * @return the current gain value.
     */
    YARP_DEPRECATED_MSG("Use getFeature(YARP_FEATURE_GAIN, v) instead")
    virtual double getGain() { return -1.0; }
    /**
     * Read the iris parameter.
     * @return the current iris value.
     */
    YARP_DEPRECATED_MSG("Use getFeature(YARP_FEATURE_IRIS, v) instead")
    virtual double getIris() { return -1.0; }
#endif

    std::string busType2String(BusType type)
    {
        switch (type) {
            case BUS_FIREWIRE:
                return "FireWire";
            break;

            case BUS_USB:
                return "USB";
            break;

            default:
                return "bus type undefined";
            break;
        }
    }

    inline FeatureMode toFeatureMode(bool _auto)
    {
        return _auto ? MODE_AUTO : MODE_MANUAL;
    }
    /**
     * Get a basic description of the camera hw. This is mainly used to determine the
     * HW bus type in order to choose the corresponding interface for advanced controls.
     * @param device returns an identifier for the bus
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool getCameraDescription(CameraDescriptor *camera)=0;

    /**
     * Check if camera has the requested feature (saturation, brightness ... )
     * @param feature the identifier of the feature to check
     * @param hasFeature flag value: true if the feature is present, false otherwise
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool hasFeature(int feature, bool *hasFeature)=0;

    /**
     * Set the requested feature to a value (saturation, brightness ... )
     * @param feature the identifier of the feature to change
     * @param value new value of the feature, range from 0 to 1 expressed as a percentage
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool setFeature(int feature, double value)=0;

    /**
     * Get the current value for the requested feature.
     * @param feature the identifier of the feature to read
     * @param value  pointer to current value of the feature, from 0 to 1 expressed as a percentage
     * @return returns true on success, false on failure.
     */
    virtual bool getFeature(int feature, double *value)=0;

    /**
     * Set the requested feature to a value using 2 params (like white balance)
     * @param feature the identifier of the feature to change
     * @param value1  first param,  from 0 to 1 expressed as a percentage
     * @param value2  second param, from 0 to 1 expressed as a percentage
     *
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool setFeature(int feature, double value1, double value2)=0;

    /**
     * Get the current value for the requested feature.
     * @param feature the identifier of the feaature to read
     * @param value1  returns the current value of the feature, from 0 to 1 expressed as a percentage
     * @param value2  returns the current value of the feature, from 0 to 1 expressed as a percentage
     * @return returns true on success, false on failure.
     */
    virtual bool getFeature(int feature, double *value1, double *value2)=0;

    /**
     * Check if the camera has the ability to turn on/off the requested feature
     * @param feature the identifier of the feature to change
     * @param hasOnOff flag true if this feature can be turned on/off, false otherwise.
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool hasOnOff(int feature, bool *HasOnOff)=0;

    /**
     * Set the requested feature on or off
     * @param feature the identifier of the feature to change
     * @param onoff true to activate, off to deactivate the feature
     * @return returns true on success, false on failure.
     */
    virtual bool setActive(int feature, bool onoff)=0;

    /**
     * Get the current status of the feature, on or off
     * @param feature the identifier of the feature to check
     * @param isActive flag true if the feature is active, false otherwise
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool getActive(int feature, bool *isActive)=0;

    /**
     * Check if the requested feature has the 'auto' mode
     * @param feature the identifier of the feature to check
     * @param hasAuto flag true if the feature is has 'auto' mode, false otherwise
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool hasAuto(int feature, bool *hasAuto)=0;

    /**
     * Check if the requested feature has the 'manual' mode
     * @param feature the identifier of the feature to check
     * @param hasAuto flag true if the feature is has 'manual' mode, false otherwise
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool hasManual(int feature, bool *hasManual)=0;

    /**
     * Check if the requested feature has the 'onePush' mode
     * @param feature the identifier of the feature to check
     * @param hasAuto flag true if the feature is has 'onePush' mode, false otherwise
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool hasOnePush(int feature, bool *hasOnePush)=0;

    /**
     * Set the requested mode for the feature
     * @param feature the identifier of the feature to change
     * @param auto_onoff true to activate 'auto' mode, false to activate 'manual' mode
     * @return returns true on success, false on failure.
     */
    virtual bool setMode(int feature, FeatureMode mode)=0;

    /**
     * Get the current mode for the feature
     * @param feature the identifier of the feature to change
     * @param hasAuto flag true if the feature is has 'auto' mode, false otherwise
     * @return returns true if success, false otherwise (e.g. the interface is not implemented)
     */
    virtual bool getMode(int feature, FeatureMode *mode)=0;

    /**
     * Set the requested feature to a value (saturation, brightness ... )
     * @param feature the identifier of the feature to change
     * @param value new value of the feature, from 0 to 1 as a percentage of param range
     * @return returns true on success, false on failure.
     */
    virtual bool setOnePush(int feature)=0;
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
