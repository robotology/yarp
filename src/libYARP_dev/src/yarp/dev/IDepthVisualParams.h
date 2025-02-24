/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IDEPTHVISUALPARAMS_H
#define YARP_DEV_IDEPTHVISUALPARAMS_H

#include <yarp/dev/api.h>
#include <yarp/os/Property.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {

/**
 * @ingroup dev_iface_other
 *
 * An interface for retrieving intrinsic parameter from a depth camera
 */
class YARP_dev_API IDepthVisualParams
{
public:
    virtual ~IDepthVisualParams();

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
     * Get the resolution of the depth image from the camera
     * @param width  image width
     * @param height image height
     * @return true on success
     */
    virtual yarp::dev::ReturnValue getDepthResolution(int& width, int& height) = 0;

    /**
     * Set the resolution of the depth image from the camera
     * @param width  image width
     * @param height image height
     * @return true on success
     */
    virtual yarp::dev::ReturnValue setDepthResolution(int width, int height) = 0;

    /**
     * Get the field of view (FOV) of the depth camera.
     *
     * @param  horizontalFov will return the value of the horizontal fov in degrees
     * @param  verticalFov   will return the value of the vertical fov in degrees
     * @return true if success
     */
    virtual yarp::dev::ReturnValue getDepthFOV(double& horizontalFov, double& verticalFov) = 0;

    /**
     * Set the field of view (FOV) of the depth camera.
     *
     * @param  horizontalFov will set the value of the horizontal fov in degrees
     * @param  verticalFov   will set the value of the vertical fov in degrees
     * @return true on success
     */
    virtual yarp::dev::ReturnValue setDepthFOV(double horizontalFov, double verticalFov) = 0;

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
    virtual yarp::dev::ReturnValue getDepthIntrinsicParam(yarp::os::Property& intrinsic) = 0;

    /**
     * Get the minimum detectable variation in distance [meter]
     * @param accuracy will return the sensor resolution in meters.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue getDepthAccuracy(double& accuracy) = 0;

    /**
     * Set the minimum detectable variation in distance [meter] when possible
     * @param the desired resolution in meters.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue setDepthAccuracy(double accuracy) = 0;

    /**
     * Get the clipping planes of the sensor
     *
     * @param nearPlane: minimum distance at which the sensor start measuring.
     *  Object closer than this distance will not be detected.
     * @param farPlane: maximum distance beyond which the sensor stop measuring.
     *  Object farther than this distance will not be detected.
     * @return true if success
     */
    virtual yarp::dev::ReturnValue getDepthClipPlanes(double& nearPlane, double& farPlane) = 0;

    /**
     * Set the clipping planes of the sensor
     *
     * @param nearPlane: minimum distance at which the sensor start measuring.
     *  Object closer than this distance will not be detected.
     * @param farPlane: maximum distance beyond which the sensor stop measuring.
     *  Object farther than this distance will not be detected.
     * @return true if success
     */
    virtual yarp::dev::ReturnValue setDepthClipPlanes(double nearPlane, double farPlane) = 0;

    /**
     * Get the mirroring setting of the sensor
     *
     * @param mirror: true if image is mirrored, false otherwise
     * @return true if success
     */
    virtual yarp::dev::ReturnValue getDepthMirroring(bool& mirror) = 0;

    /**
     * Set the mirroring setting of the sensor
     *
     * @param mirror: true if image should be mirrored, false otherwise
     * @return true if success
     */
    virtual yarp::dev::ReturnValue setDepthMirroring(bool mirror) = 0;
};

} // namespace yarp::dev

#endif // YARP_DEV_IDEPTHVISUALPARAMS_H
