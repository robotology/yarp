/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IRGBVISUALPARAMS_H
#define YARP_DEV_IRGBVISUALPARAMS_H

#include <yarp/dev/api.h>

#include <yarp/os/Property.h>

#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Portable.h>

#include <yarp/sig/Image.h>
#include <yarp/sig/IntrinsicParams.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/ReturnValue.h>

#include <yarp/dev/CameraConfigData.h>

namespace yarp::dev {

typedef CameraConfigData CameraConfig;

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
    virtual yarp::dev::ReturnValue getRgbSupportedConfigurations(std::vector<yarp::dev::CameraConfigData>& configurations) = 0;

    /**
     * Get the resolution of the rgb image from the camera
     * @param width  image width
     * @param height image height
     * @return true on success
     */
    virtual yarp::dev::ReturnValue getRgbResolution(int& width, int& height) = 0;

    /**
     * Set the resolution of the rgb image from the camera
     * @param width  image width
     * @param height image height
     * @return true on success
     */
    virtual yarp::dev::ReturnValue setRgbResolution(int width, int height) = 0;

    /**
     * Get the field of view (FOV) of the rgb camera.
     *
     * @param  horizontalFov will return the value of the horizontal fov in degrees
     * @param  verticalFov   will return the value of the vertical fov in degrees
     * @return true on success
     */
    virtual yarp::dev::ReturnValue getRgbFOV(double& horizontalFov, double& verticalFov) = 0;

    /**
     * Set the field of view (FOV) of the rgb camera.
     *
     * @param  horizontalFov will set the value of the horizontal fov in degrees
     * @param  verticalFov   will set the value of the vertical fov in degrees
     * @return true on success
     */
    virtual yarp::dev::ReturnValue setRgbFOV(double horizontalFov, double verticalFov) = 0;

    /**
     * Get the intrinsic parameters of the rgb camera
     * @param  intrinsic  return a yarp::sig::IntrinsicParams containing intrinsic parameters
     *       of the optical model of the camera.
     * @return true if success
    */
    virtual yarp::dev::ReturnValue getRgbIntrinsicParam(yarp::sig::IntrinsicParams& intrinsic) = 0;

    /**
     * Get the mirroring setting of the sensor
     *
     * @param mirror: true if image is mirrored, false otherwise
     * @return true if success
     */
    virtual yarp::dev::ReturnValue getRgbMirroring(bool& mirror) = 0;

    /**
     * Set the mirroring setting of the sensor
     *
     * @param mirror: true if image should be mirrored, false otherwise
     * @return true if success
     */
    virtual yarp::dev::ReturnValue setRgbMirroring(bool mirror) = 0;
};

} // namespace yarp::dev

#endif // YARP_DEV_IRGBVISUALPARAMS_H
