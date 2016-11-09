/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */


#ifndef YARP_VISUAL_PARAMS_INTERFACE
#define YARP_VISUAL_PARAMS_INTERFACE

#include <yarp/os/Property.h>

namespace yarp {
    namespace dev {
        class IRgbVisualParams;
        class IDepthVisualParams;
    }
}

/**
 * @ingroup dev_iface_other
 *
 * An interface for retriving intrinsic parameter from a rgb camera
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
    virtual bool setRgbResolution(int width, int height) = 0;

    /**
     * Get the field of view (FOV) of the rgb camera.
     *
     * @param  horizontalFov will return the value of the horizontal fov
     * @param  verticalFov   will return the value of the vertical fov
     * @return true if success
     */
    virtual bool getRgbFOV(double &horizontalFov, double &verticalFov) = 0;
    virtual bool setRgbFOV(double horizontalFov, double verticalFov) = 0;

    /**
     * Get the intrinsic parameters of the rgb camera
     * @param  intrinsic  return a Property containing intrinsic parameters
     *       of the optical model of the camera.
     * @return true if success
     */
    virtual bool getRgbIntrinsicParam(yarp::os::Property &intrinsic) = 0;
};


/**
 * @ingroup dev_iface_other
 *
 * An interface for retriving intrinsic parameter from a depth camera
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
    virtual bool setDepthResolution(int width, int height) = 0;

    /**
     * Get the field of view (FOV) of the depth camera.
     *
     * @param  horizontalFov will return the value of the horizontal fov
     * @param  verticalFov   will return the value of the vertical fov
     * @return true if success
     */
    virtual bool getDepthFOV(double &horizontalFov, double &verticalFov) = 0;
    virtual bool setDepthFOV(double horizontalFov, double verticalFov) = 0;

    /**
     * Get the intrinsic parameters of the depth camera
     * @param  intrinsic  return a Property containing intrinsic parameters
     *       of the optical model of the camera.
     * @return true if success
     */
    virtual bool getDepthIntrinsicParam(yarp::os::Property &intrinsic) = 0;

    /**
     * Get the accuracy of the depth measure in [meter]
     * @return the accuracy of the sensor in meters.
     */
    virtual double getDepthAccuracy() = 0;
    virtual bool   setDepthAccuracy(double accuracy) = 0;

    /**
     * Get the clipping planes of the sensor
     *
     * @param near: minimum distance at which the sensor start measuring.
     *  Object closer than this distance will not be detected.
     * @param far: maximum distance beyond which the sensor stop measuring.
     *  Object farther than this distance will not be detected.
     * @return true if success
     */
    virtual bool getDepthClipPlanes(double &near, double &far) = 0;

    /**
     * Set the clipping planes of the sensor
     *
     * @param near: minimum distance at which the sensor start measuring.
     *  Object closer than this distance will not be detected.
     * @param far: maximum distance beyond which the sensor stop measuring.
     *  Object farther than this distance will not be detected.
     * @return true if success
     */
    virtual bool setDepthClipPlanes(double near, double far) = 0;

};

#endif   // YARP_VISUAL_PARAMS_INTERFACE
