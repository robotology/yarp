/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_FRAMEGRABBERINTERFACES_H
#define YARP_FRAMEGRABBERINTERFACES_H

#include <string>

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>

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

/*
 *  Vocab for interfaces
 */
constexpr yarp::conf::vocab32_t VOCAB_FRAMEGRABBER_IMAGE        = yarp::os::createVocab('f','g','i');
constexpr yarp::conf::vocab32_t VOCAB_FRAMEGRABBER_IMAGERAW     = yarp::os::createVocab('f','g','i','r');

/*
 * Generic capabilities defines
 */

constexpr yarp::conf::vocab32_t VOCAB_BRIGHTNESS                = yarp::os::createVocab('b','r','i');
constexpr yarp::conf::vocab32_t VOCAB_EXPOSURE                  = yarp::os::createVocab('e','x','p','o');
constexpr yarp::conf::vocab32_t VOCAB_SHARPNESS                 = yarp::os::createVocab('s','h','a','r');
constexpr yarp::conf::vocab32_t VOCAB_WHITE                     = yarp::os::createVocab('w','h','i','t');
constexpr yarp::conf::vocab32_t VOCAB_HUE                       = yarp::os::createVocab('h','u','e');
constexpr yarp::conf::vocab32_t VOCAB_SATURATION                = yarp::os::createVocab('s','a','t','u');
constexpr yarp::conf::vocab32_t VOCAB_GAMMA                     = yarp::os::createVocab('g','a','m','m');
constexpr yarp::conf::vocab32_t VOCAB_SHUTTER                   = yarp::os::createVocab('s','h','u','t');
constexpr yarp::conf::vocab32_t VOCAB_GAIN                      = yarp::os::createVocab('g','a','i','n');
constexpr yarp::conf::vocab32_t VOCAB_IRIS                      = yarp::os::createVocab('i','r','i','s');

// General usage vocabs
constexpr yarp::conf::vocab32_t VOCAB_CROP                      = yarp::os::createVocab('c','r','o','p');
constexpr yarp::conf::vocab32_t VOCAB_FRAMEGRABBER_CONTROL      = yarp::os::createVocab('f','g','c');
constexpr yarp::conf::vocab32_t VOCAB_FRAMEGRABBER_CONTROL_DC1394 = yarp::os::createVocab('f','g','f','w');
constexpr yarp::conf::vocab32_t VOCAB_CAMERA_DESCRIPTION        = yarp::os::createVocab('c','a','m','d');
constexpr yarp::conf::vocab32_t VOCAB_HAS                       = yarp::os::createVocab('h','a','s');
constexpr yarp::conf::vocab32_t VOCAB_FEATURE                   = yarp::os::createVocab('f','e','a','t');
constexpr yarp::conf::vocab32_t VOCAB_FEATURE2                  = yarp::os::createVocab('f','e','a','2');
constexpr yarp::conf::vocab32_t VOCAB_ONOFF                     = yarp::os::createVocab('o','n','o','f');
constexpr yarp::conf::vocab32_t VOCAB_AUTO                      = yarp::os::createVocab('a','u','t','o');
constexpr yarp::conf::vocab32_t VOCAB_MANUAL                    = yarp::os::createVocab('m','a','n');
constexpr yarp::conf::vocab32_t VOCAB_ONEPUSH                   = yarp::os::createVocab('o','n','e','p');
constexpr yarp::conf::vocab32_t VOCAB_ACTIVE                    = yarp::os::createVocab('a','c','t','v');
constexpr yarp::conf::vocab32_t VOCAB_MODE                      = yarp::os::createVocab('m','o','d','e');


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

/*
 * For usage with IFrameGrabberControlsDC1394 interface
 */

constexpr yarp::conf::vocab32_t VOCAB_DRHASFEA = yarp::os::createVocab('D','R','2','a');// 00
constexpr yarp::conf::vocab32_t VOCAB_DRSETVAL = yarp::os::createVocab('D','R','2','b');// 01
constexpr yarp::conf::vocab32_t VOCAB_DRGETVAL = yarp::os::createVocab('D','R','2','c');// 02
constexpr yarp::conf::vocab32_t VOCAB_DRHASACT = yarp::os::createVocab('D','R','2','d');// 03
constexpr yarp::conf::vocab32_t VOCAB_DRSETACT = yarp::os::createVocab('D','R','2','e');// 04
constexpr yarp::conf::vocab32_t VOCAB_DRGETACT = yarp::os::createVocab('D','R','2','f');// 05
constexpr yarp::conf::vocab32_t VOCAB_DRHASMAN = yarp::os::createVocab('D','R','2','g');// 06
constexpr yarp::conf::vocab32_t VOCAB_DRHASAUT = yarp::os::createVocab('D','R','2','h');// 07
constexpr yarp::conf::vocab32_t VOCAB_DRHASONP = yarp::os::createVocab('D','R','2','i');// 08
constexpr yarp::conf::vocab32_t VOCAB_DRSETMOD = yarp::os::createVocab('D','R','2','j');// 09
constexpr yarp::conf::vocab32_t VOCAB_DRGETMOD = yarp::os::createVocab('D','R','2','k');// 10
constexpr yarp::conf::vocab32_t VOCAB_DRSETONP = yarp::os::createVocab('D','R','2','l');// 11

// masks
constexpr yarp::conf::vocab32_t VOCAB_DRGETMSK = yarp::os::createVocab('D','R','2','m'); // 12
constexpr yarp::conf::vocab32_t VOCAB_DRGETVMD = yarp::os::createVocab('D','R','2','n'); // 13
constexpr yarp::conf::vocab32_t VOCAB_DRSETVMD = yarp::os::createVocab('D','R','2','o'); // 14
constexpr yarp::conf::vocab32_t VOCAB_DRGETFPM = yarp::os::createVocab('D','R','2','p'); // 15
constexpr yarp::conf::vocab32_t VOCAB_DRGETFPS = yarp::os::createVocab('D','R','2','q'); // 16
constexpr yarp::conf::vocab32_t VOCAB_DRSETFPS = yarp::os::createVocab('D','R','2','r'); // 17
constexpr yarp::conf::vocab32_t VOCAB_DRGETISO = yarp::os::createVocab('D','R','2','s'); // 18
constexpr yarp::conf::vocab32_t VOCAB_DRSETISO = yarp::os::createVocab('D','R','2','t'); // 19
constexpr yarp::conf::vocab32_t VOCAB_DRGETCCM = yarp::os::createVocab('D','R','2','u'); // 20
constexpr yarp::conf::vocab32_t VOCAB_DRGETCOD = yarp::os::createVocab('D','R','2','v'); // 21
constexpr yarp::conf::vocab32_t VOCAB_DRSETCOD = yarp::os::createVocab('D','R','2','w'); // 22
constexpr yarp::conf::vocab32_t VOCAB_DRSETWHB = yarp::os::createVocab('D','R','2','x'); // 23
constexpr yarp::conf::vocab32_t VOCAB_DRGETWHB = yarp::os::createVocab('D','R','2','y'); // 24
constexpr yarp::conf::vocab32_t VOCAB_DRGETF7M = yarp::os::createVocab('D','R','2','z'); // 25
constexpr yarp::conf::vocab32_t VOCAB_DRGETWF7 = yarp::os::createVocab('D','R','2','A'); // 26
constexpr yarp::conf::vocab32_t VOCAB_DRSETWF7 = yarp::os::createVocab('D','R','2','B'); // 27
constexpr yarp::conf::vocab32_t VOCAB_DRSETOPM = yarp::os::createVocab('D','R','2','C'); // 28
constexpr yarp::conf::vocab32_t VOCAB_DRGETOPM = yarp::os::createVocab('D','R','2','D'); // 29
constexpr yarp::conf::vocab32_t VOCAB_DRSETTXM = yarp::os::createVocab('D','R','2','E'); // 30
constexpr yarp::conf::vocab32_t VOCAB_DRGETTXM = yarp::os::createVocab('D','R','2','F'); // 31


constexpr yarp::conf::vocab32_t VOCAB_DRSETBCS = yarp::os::createVocab('D','R','2','I'); // 34
constexpr yarp::conf::vocab32_t VOCAB_DRSETDEF = yarp::os::createVocab('D','R','2','J'); // 35
constexpr yarp::conf::vocab32_t VOCAB_DRSETRST = yarp::os::createVocab('D','R','2','K'); // 36
constexpr yarp::conf::vocab32_t VOCAB_DRSETPWR = yarp::os::createVocab('D','R','2','L'); // 37
constexpr yarp::conf::vocab32_t VOCAB_DRSETCAP = yarp::os::createVocab('D','R','2','M'); // 38
constexpr yarp::conf::vocab32_t VOCAB_DRSETBPP = yarp::os::createVocab('D','R','2','N'); // 39
constexpr yarp::conf::vocab32_t VOCAB_DRGETBPP = yarp::os::createVocab('D','R','2','O'); // 40

namespace yarp{
namespace dev {

/**
 * @ingroup dev_iface_media
 *
 * Common interface to a FrameGrabber.
 */
class YARP_dev_API IFrameGrabber
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
class YARP_dev_API IFrameGrabberRgb
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

    cameraFeature_id_t featureVOCABEnum(int vocab)
    {
        switch (vocab) {
        case VOCAB_BRIGHTNESS:
            return YARP_FEATURE_BRIGHTNESS;
        case VOCAB_EXPOSURE:
            return YARP_FEATURE_EXPOSURE;
        case VOCAB_SHARPNESS:
            return YARP_FEATURE_SHARPNESS;
        case VOCAB_WHITE:
            return YARP_FEATURE_WHITE_BALANCE;
        case VOCAB_HUE:
            return YARP_FEATURE_HUE;
        case VOCAB_SATURATION:
            return YARP_FEATURE_SATURATION;
        case VOCAB_GAMMA:
            return YARP_FEATURE_GAMMA;
        case VOCAB_SHUTTER:
            return YARP_FEATURE_SHUTTER;
        case VOCAB_GAIN:
            return YARP_FEATURE_GAIN;
        case VOCAB_IRIS:
            return YARP_FEATURE_IRIS;
        default:
            return YARP_FEATURE_INVALID;
        }

    }

    int featureEnum2Vocab(cameraFeature_id_t _enum)
    {
        switch (_enum) {
        case YARP_FEATURE_BRIGHTNESS:
            return VOCAB_BRIGHTNESS;
        case YARP_FEATURE_EXPOSURE:
            return VOCAB_EXPOSURE;
        case YARP_FEATURE_SHARPNESS:
            return VOCAB_SHARPNESS;
        case YARP_FEATURE_WHITE_BALANCE:
            return VOCAB_WHITE;
        case YARP_FEATURE_HUE:
            return VOCAB_HUE;
        case YARP_FEATURE_SATURATION:
            return VOCAB_SATURATION;
        case YARP_FEATURE_GAMMA:
            return VOCAB_GAMMA;
        case YARP_FEATURE_SHUTTER:
            return VOCAB_SHUTTER;
        case YARP_FEATURE_GAIN:
            return VOCAB_GAIN;
        case YARP_FEATURE_IRIS:
            return VOCAB_IRIS;
        default:
            return -1;
        }

    }

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
