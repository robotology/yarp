/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IRGBVISUALPARAMS_H
#define YARP_DEV_IRGBVISUALPARAMS_H

#include <yarp/dev/api.h>

#include <yarp/os/Property.h>

#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>

namespace yarp {
namespace dev {

/**
 * Struct describing a possible camera configuration
 * @param width image width
 * @param height image height
 * @param framerate camera framerate
 * @param pixelCoding camera pixel coding
 */
YARP_BEGIN_PACK
struct CameraConfig
{
    int width {0};
    int height {0};
    double framerate {0.0};
    YarpVocabPixelTypesEnum pixelCoding {VOCAB_PIXEL_INVALID};
};
YARP_END_PACK

/**
 * @ingroup dev_iface_other
 *
 * An interface for retrieving intrinsic parameter from a rgb camera
 */
class YARP_dev_API IRgbVisualParams
{
public:
    virtual ~IRgbVisualParams();

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
    virtual bool getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig>& configurations)
    {
        YARP_UNUSED(configurations);
        return false;
    }

    /**
     * Get the resolution of the rgb image from the camera
     * @param width  image width
     * @param height image height
     * @return true on success
     */
    virtual bool getRgbResolution(int& width, int& height)
    {
        YARP_UNUSED(width);
        YARP_UNUSED(height);
        return false;
    }

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
    virtual bool getRgbFOV(double& horizontalFov, double& verticalFov) = 0;

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
     * |  rectificationMatrix         |      -              | 4x4 double matrix   | -              |   -           |   Yes                            |  Matrix that describes the lens' distortion                                            |                                                                       |
     * |  distortionModel             |      -              | string              | -              |   -           |   Yes                            |  Reference to group of parameters describing the distortion model of the camera, example 'cameraDistortionModelGroup'       | This is another group's name to be searched for in the config file    |
     * |  cameraDistortionModelGroup  |                     |                     |                |               |                                  |                                                                                        |                                                                       |
     * |                              |   name              | string              | -              |   -           |   Yes                            |  Name of the distortion model, see notes                                               | right now only 'plumb_bob' is supported                               |
     * |                              |   k1                | double              | -              |   -           |   Yes                            |  Radial distortion coefficient of the lens                                             |                                                                       |
     * |                              |   k2                | double              | -              |   -           |   Yes                            |  Radial distortion coefficient of the lens                                             |                                                                       |
     * |                              |   k3                | double              | -              |   -           |   Yes                            |  Radial distortion coefficient of the lens                                             |                                                                       |
     * |                              |   t1                | double              | -              |   -           |   Yes                            |  Tangential distortion of the lens                                                     |                                                                       |
     * |                              |   t2                | double              | -              |   -           |   Yes                            |  Tangential distortion of the lens                                                     |                                                                       |
     */
    virtual bool getRgbIntrinsicParam(yarp::os::Property& intrinsic) = 0;

    /**
     * Get the mirroring setting of the sensor
     *
     * @param mirror: true if image is mirrored, false otherwise
     * @return true if success
     */
    virtual bool getRgbMirroring(bool& mirror) = 0;

    /**
     * Set the mirroring setting of the sensor
     *
     * @param mirror: true if image should be mirrored, false otherwise
     * @return true if success
     */
    virtual bool setRgbMirroring(bool mirror) = 0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_IRGBVISUALPARAMS_H
