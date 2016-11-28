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

#include <yarp/os/Vocab.h>
#include <yarp/os/Property.h>


namespace yarp {
    namespace dev {
        class IRgbVisualParams;
        class IDepthVisualParams;
    }
}

// Interface name
#define VOCAB_RGB_VISUAL_PARAMS       VOCAB4('v','i','s','r')
#define VOCAB_DEPTH_VISUAL_PARAMS     VOCAB4('v','i','s','d')

// Common
#define VOCAB_SET               VOCAB3('s','e','t')
#define VOCAB_GET               VOCAB3('g','e','t')
#define VOCAB_IS                VOCAB2('i','s')
#define VOCAB_OK                VOCAB2('o','k')
#define VOCAB_FAILED            VOCAB4('f','a','i','l')

// Rgb and depth
#define VOCAB_RGB               VOCAB3('r','g','b')
#define VOCAB_DEPTH             VOCAB4('d','e','p','t')
#define VOCAB_MIRROR            VOCAB4('m','i','r','r')

// Methods
#define VOCAB_WIDTH             VOCAB1('w')
#define VOCAB_HEIGHT            VOCAB1('h')
#define VOCAB_RESOLUTION        VOCAB3('r','e','s')
#define VOCAB_FOV               VOCAB3('f','o','v')
#define VOCAB_INTRINSIC_PARAM   VOCAB4('i','n','t','p')

// Depth only
#define VOCAB_ACCURACY          VOCAB4('a','c','r','c')
#define VOCAB_CLIP_PLANES       VOCAB4('c','l','i','p')


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
     */
    virtual bool getDepthIntrinsicParam(yarp::os::Property &intrinsic) = 0;

    /**
     * Get the accuracy of the depth measure in [meter]
     * @return the accuracy of the sensor in meters.
     */
    virtual double getDepthAccuracy() = 0;

    /**
     * Set the accuracy of the depth measure in [meter] when possible
     * @param the accuracy of the sensor in meters.
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

#endif   // YARP_VISUAL_PARAMS_INTERFACE
