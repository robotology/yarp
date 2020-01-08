/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IVISUALPARAMS_H
#define YARP_DEV_IVISUALPARAMS_H

#include <yarp/dev/api.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Property.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>

namespace yarp {
    namespace dev {
        class IRgbVisualParams;
        class IDepthVisualParams;
        struct CameraConfig;
    }
}
/**
 * Struct describing a possible camera configuration
 * @param width image width
 * @param height image height
 * @param framerate camera framerate
 * @param pixelCoding camera pixel coding
 */
YARP_BEGIN_PACK
struct yarp::dev::CameraConfig {
    int width;
    int height;
    double framerate;
    YarpVocabPixelTypesEnum pixelCoding;

    CameraConfig() : width(0), height(0), framerate(0.0), pixelCoding(VOCAB_PIXEL_INVALID) {}
};
YARP_END_PACK

// Interface name
constexpr yarp::conf::vocab32_t VOCAB_RGB_VISUAL_PARAMS       = yarp::os::createVocab('v','i','s','r');
constexpr yarp::conf::vocab32_t VOCAB_DEPTH_VISUAL_PARAMS     = yarp::os::createVocab('v','i','s','d');

// Common

// Rgb and depth
constexpr yarp::conf::vocab32_t VOCAB_RGB               = yarp::os::createVocab('r','g','b');
constexpr yarp::conf::vocab32_t VOCAB_DEPTH             = yarp::os::createVocab('d','e','p','t');
constexpr yarp::conf::vocab32_t VOCAB_MIRROR            = yarp::os::createVocab('m','i','r','r');

// Methods
constexpr yarp::conf::vocab32_t VOCAB_RESOLUTION        = yarp::os::createVocab('r','e','s');
constexpr yarp::conf::vocab32_t VOCAB_FOV               = yarp::os::createVocab('f','o','v');
constexpr yarp::conf::vocab32_t VOCAB_INTRINSIC_PARAM   = yarp::os::createVocab('i','n','t','p');
constexpr yarp::conf::vocab32_t VOCAB_SUPPORTED_CONF    = yarp::os::createVocab('c','o','n','f');

// Depth only
constexpr yarp::conf::vocab32_t VOCAB_ACCURACY          = yarp::os::createVocab('a','c','r','c');
constexpr yarp::conf::vocab32_t VOCAB_CLIP_PLANES       = yarp::os::createVocab('c','l','i','p');



/**
 * @ingroup dev_iface_other
 *
 * An interface for retrieving intrinsic parameter from a rgb camera
 *
 */
class YARP_dev_API yarp::dev::IRgbVisualParams
{
public:
    virtual ~IRgbVisualParams(){}

    /**
     * Return the height of each frame.
     * @return rgb image height
     */
    virtual int getRgbHeight() = 0;

    /**
     * Return the width of each frame.
     * @return rgb image width
     */
    virtual int getRgbWidth() = 0;

    /**
     * Get the possible configurations of the camera
     * @param configurations  list of camera supported configurations as CameraConfig type
     * @return true on success
     */

    virtual bool getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig> &configurations) {return false;}
    /**
     * Get the resolution of the rgb image from the camera
     * @param width  image width
     * @param height image height
     * @return true on success
     */

    virtual bool getRgbResolution(int &width, int &height) {return false;}
    /**
     * Set the resolution of the rgb image from the camera
     * @param width  image width
     * @param height image height
     * @return true on success
     */
    virtual bool setRgbResolution(int width, int height) = 0;

    /**
     * Get the field of view (FOV) of the rgb camera.
     *
     * @param  horizontalFov will return the value of the horizontal fov in degrees
     * @param  verticalFov   will return the value of the vertical fov in degrees
     * @return true on success
     */
    virtual bool getRgbFOV(double &horizontalFov, double &verticalFov) = 0;

    /**
     * Set the field of view (FOV) of the rgb camera.
     *
     * @param  horizontalFov will set the value of the horizontal fov in degrees
     * @param  verticalFov   will set the value of the vertical fov in degrees
     * @return true on success
     */
    virtual bool setRgbFOV(double horizontalFov, double verticalFov) = 0;

    /**
     * Get the intrinsic parameters of the rgb camera
     * @param  intrinsic  return a Property containing intrinsic parameters
     *       of the optical model of the camera.
     * @return true if success
     *
     * The yarp::os::Property describing the intrinsic parameters is expected to be
     * in the form:
     *
     * |  Parameter name              | SubParameter        | Type                | Units          | Default Value | Required                         | Description                                                                            | Notes                                                                 |
     * |:----------------------------:|:-------------------:|:-------------------:|:--------------:|:-------------:|:--------------------------------:|:--------------------------------------------------------------------------------------:|:---------------------------------------------------------------------:|
     * |  physFocalLength             |      -              | double              | m              |   -           |   Yes                            |  Physical focal length of the lens in meters                                           |                                                                       |
     * |  focalLengthX                |      -              | double              | pixel          |   -           |   Yes                            |  Horizontal component of the focal length as a multiple of pixel width                 |                                                                       |
     * |  focalLengthY                |      -              | double              | pixel          |   -           |   Yes                            |  Vertical component of the focal length as a multiple of pixel height                  |                                                                       |
     * |  principalPointX             |      -              | double              | pixel          |   -           |   Yes                            |  X coordinate of the principal point                                                   |                                                                       |
     * |  principalPointY             |      -              | double              | pixel          |   -           |   Yes                            |  Y coordinate of the principal point                                                   |                                                                       |
     * |  retificationMatrix          |      -              | 4x4 double matrix   | -              |   -           |   Yes                            |  Matrix that describes the lens' distortion                                            |                                                                       |
     * |  distortionModel             |      -              | string              | -              |   -           |   Yes                            |  Reference to group of parameters describing the distortion model of the camera, example 'cameraDistortionModelGroup'       | This is another group's name to be searched for in the config file    |
     * |  cameraDistortionModelGroup  |                     |                     |                |               |                                  |                                                                                        |                                                                       |
     * |                              |   name              | string              | -              |   -           |   Yes                            |  Name of the distortion model, see notes                                               | right now only 'plumb_bob' is supported                               |
     * |                              |   k1                | double              | -              |   -           |   Yes                            |  Radial distortion coefficient of the lens                                             |                                                                       |
     * |                              |   k2                | double              | -              |   -           |   Yes                            |  Radial distortion coefficient of the lens                                             |                                                                       |
     * |                              |   k3                | double              | -              |   -           |   Yes                            |  Radial distortion coefficient of the lens                                             |                                                                       |
     * |                              |   t1                | double              | -              |   -           |   Yes                            |  Tangential distortion of the lens                                                     |                                                                       |
     * |                              |   t2                | double              | -              |   -           |   Yes                            |  Tangential distortion of the lens                                                     |                                                                       |
     */
    virtual bool getRgbIntrinsicParam(yarp::os::Property &intrinsic) = 0;

    /**
     * Get the mirroring setting of the sensor
     *
     * @param mirror: true if image is mirrored, false otherwise
     * @return true if success
     */
    virtual bool getRgbMirroring(bool &mirror) = 0;

    /**
     * Set the mirroring setting of the sensor
     *
     * @param mirror: true if image should be mirrored, false otherwise
     * @return true if success
     */
    virtual bool setRgbMirroring(bool mirror) = 0;
};


/**
 * @ingroup dev_iface_other
 *
 * An interface for retrieving intrinsic parameter from a depth camera
 *
 */
class YARP_dev_API yarp::dev::IDepthVisualParams
{
public:

    virtual ~IDepthVisualParams(){}
    /**
     * Return the height of each frame.
     * @return depth image height
     */
    virtual int getDepthHeight() = 0;

    /**
     * Return the height of each frame.
     * @return depth image height
     */
    virtual int getDepthWidth() = 0;

    /**
     * Set the resolution of the depth image from the camera
     * @param width  image width
     * @param height image height
     * @return true on success
     */
    virtual bool setDepthResolution(int width, int height) = 0;

    /**
     * Get the field of view (FOV) of the depth camera.
     *
     * @param  horizontalFov will return the value of the horizontal fov in degrees
     * @param  verticalFov   will return the value of the vertical fov in degrees
     * @return true if success
     */
    virtual bool getDepthFOV(double &horizontalFov, double &verticalFov) = 0;

    /**
     * Set the field of view (FOV) of the depth camera.
     *
     * @param  horizontalFov will set the value of the horizontal fov in degrees
     * @param  verticalFov   will set the value of the vertical fov in degrees
     * @return true on success
     */
    virtual bool setDepthFOV(double horizontalFov, double verticalFov) = 0;

    /**
     * Get the intrinsic parameters of the depth camera
     * @param  intrinsic  return a Property containing intrinsic parameters
     *       of the optical model of the camera.
     * @return true if success
     *
     * The yarp::os::Property describing the intrinsic parameters is expected to be
     * in the form:
     *
     * |  Parameter name              | SubParameter        | Type                | Units          | Default Value | Required                         | Description                                                                            | Notes                                                                 |
     * |:----------------------------:|:-------------------:|:-------------------:|:--------------:|:-------------:|:--------------------------------:|:--------------------------------------------------------------------------------------:|:---------------------------------------------------------------------:|
     * |  physFocalLength             |      -              | double              | m              |   -           |   Yes                            |  Physical focal length of the lens in meters                                           |                                                                       |
     * |  focalLengthX                |      -              | double              | pixel          |   -           |   Yes                            |  Horizontal component of the focal length as a multiple of pixel width                 |                                                                       |
     * |  focalLengthY                |      -              | double              | pixel          |   -           |   Yes                            |  Vertical component of the focal length as a multiple of pixel height                  |                                                                       |
     * |  principalPointX             |      -              | double              | pixel          |   -           |   Yes                            |  X coordinate of the principal point                                                   |                                                                       |
     * |  principalPointY             |      -              | double              | pixel          |   -           |   Yes                            |  Y coordinate of the principal point                                                   |                                                                       |
     * |  retificationMatrix          |      -              | 4x4 double matrix   | -              |   -           |   Yes                            |  Matrix that describes the lens' distortion                                            |                                                                       |
     * |  distortionModel             |      -              | string              | -              |   -           |   Yes                            |  Reference to group of parameters describing the distortion model of the camera, example 'cameraDistortionModelGroup'       | This is another group's name to be searched for in the config file    |
     * |  cameraDistortionModelGroup  |                     |                     |                |               |                                  |                                                                                        |                                                                       |
     * |                              |   name              | string              | -              |   -           |   Yes                            |  Name of the distortion model, see notes                                               | right now only 'plumb_bob' is supported                               |
     * |                              |   k1                | double              | -              |   -           |   Yes                            |  Radial distortion coefficient of the lens                                             |                                                                       |
     * |                              |   k2                | double              | -              |   -           |   Yes                            |  Radial distortion coefficient of the lens                                             |                                                                       |
     * |                              |   k3                | double              | -              |   -           |   Yes                            |  Radial distortion coefficient of the lens                                             |                                                                       |
     * |                              |   t1                | double              | -              |   -           |   Yes                            |  Tangential distortion of the lens                                                     |                                                                       |
     * |                              |   t2                | double              | -              |   -           |   Yes                            |  Tangential distortion of the lens                                                     |                                                                       |
     */
    virtual bool getDepthIntrinsicParam(yarp::os::Property &intrinsic) = 0;

    /**
     * Get the minimum detectable variation in distance [meter]
     * @return the sensor resolution in meters.
     */
    virtual double getDepthAccuracy() = 0;

    /**
     * Set the minimum detectable variation in distance [meter] when possible
     * @param the desired resolution in meters.
     * @return true on success
     */
    virtual bool setDepthAccuracy(double accuracy) = 0;

    /**
     * Get the clipping planes of the sensor
     *
     * @param nearPlane: minimum distance at which the sensor start measuring.
     *  Object closer than this distance will not be detected.
     * @param farPlane: maximum distance beyond which the sensor stop measuring.
     *  Object farther than this distance will not be detected.
     * @return true if success
     */
    virtual bool getDepthClipPlanes(double &nearPlane, double &farPlane) = 0;

    /**
     * Set the clipping planes of the sensor
     *
     * @param nearPlane: minimum distance at which the sensor start measuring.
     *  Object closer than this distance will not be detected.
     * @param farPlane: maximum distance beyond which the sensor stop measuring.
     *  Object farther than this distance will not be detected.
     * @return true if success
     */
    virtual bool setDepthClipPlanes(double nearPlane, double farPlane) = 0;

    /**
     * Get the mirroring setting of the sensor
     *
     * @param mirror: true if image is mirrored, false otherwise
     * @return true if success
     */
    virtual bool getDepthMirroring(bool &mirror) = 0;

    /**
     * Set the mirroring setting of the sensor
     *
     * @param mirror: true if image should be mirrored, false otherwise
     * @return true if success
     */
    virtual bool setDepthMirroring(bool mirror) = 0;


};

#endif   // YARP_DEV_IVISUALPARAMS_H
