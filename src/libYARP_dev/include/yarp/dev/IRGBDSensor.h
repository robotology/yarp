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


#ifndef YARP_DEV_RGBD_SENSOR_INTERFACE
#define YARP_DEV_RGBD_SENSOR_INTERFACE

#include <yarp/os/Stamp.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IVisualParams.h>

namespace yarp {
    namespace dev {
        class IRGBDSensor;
    }
}


#define VOCAB_SET       VOCAB3('s','e','t')
#define VOCAB_GET       VOCAB3('g','e','t')
#define VOCAB_IS        VOCAB2('i','s')
#define VOCAB_OK        VOCAB2('o','k')
#define VOCAB_FAILED    VOCAB4('f','a','i','l')

/**
 * @ingroup dev_iface_other
 *
 * A generic interface for cameras that have both color camera as well
 * as depth camera sensor, like kinect device.
 *
 * This interface uses the existing IRgbVisualParams and IDepthVisualParams
 * interfaces to retrieve the information about the cameras.
 * This device extends the functionality by adding methods for getting
 * extrinsic parameters and to actually retrieve the images.
 * The images can be synchronized on client side by using the PortSynchronizer
 * object.
 * How the synch policy is implemented depends on the client device, custom policy
 * can be used.
 */

class YARP_dev_API yarp::dev::IRGBDSensor : public IRgbVisualParams,
                                            public IDepthVisualParams
{
public:

    // We should distinguish between rgb and depth sensors, which one is working
    // and which one isn't. Maybe a specific function with two separated values is better.
    // Here values are referred to the sensor as a whole.
    enum RGBDSensor_status
    {
        RGBD_SENSOR_NOT_READY        = 0,
        RGBD_SENSOR_OK_STANBY        = 1,
        RGBD_SENSOR_OK_IN_USE        = 2,
        RGB_SENSOR_ERROR             = 3,
        DEPTH_SENSOR_ERROR           = 4,
        RGBD_SENSOR_GENERIC_ERROR    = 5,
        RGBD_SENSOR_TIMEOUT          = 6
    };

    virtual ~IRGBDSensor(){}

    /*
     *  IRgbVisualParams interface. Look at IVisualParams.h for documentation
     */
    virtual int  getRgbHeight() = 0;
    virtual int  getRgbWidth() = 0;
    virtual bool getRgbFOV(int &horizontalFov, int &verticalFov) = 0;
    virtual bool getRgbIntrinsicParam(yarp::os::Property &intrinsic) = 0;
    virtual bool getRgbSensorInfo(yarp::os::Property &info) = 0;

    /*
     * IDepthVisualParams interface. Look at IVisualParams.h for documentation
     */
    virtual int    getDepthHeight() = 0;
    virtual int    getDepthWidth() = 0;
    virtual bool   getDepthFOV(int &horizontalFov, int &verticalFov) = 0;
    virtual bool   getDepthIntrinsicParam(yarp::os::Property &intrinsic) = 0;
    virtual bool   getDepthSensorInfo(yarp::os::Property info) = 0;
    virtual double getDepthAccuracy() = 0;
    virtual bool   getDepthClipPlanes(int &near, int &far) = 0;
    virtual bool   setDepthClipPlanes(int near, int far) = 0;

    /*
     * IRGBDSensor specific interface methods
     */

    /**
     * Get the extrinsic parameters of the rgb camera
     * @param  extrinsic  return a Property containing extrinsic parameters
     *       of the optical model of the camera.
     * @return true if success
     */
    virtual bool getExtrinsicParam(yarp::os::Property &extrinsic) = 0;

    /**
     * Return an error message in case of error. For debugging purpose and user notification.
     * Error message will be reset after any succesful command
     * @return A string explaining the last error occurred.
     */
    virtual yarp::os::ConstString getLastErrorMsg(yarp::os::Stamp *timeStamp = NULL) = 0;

    /**
     * Get the rgb frame from the device.
     * The pixel type of the source image will usually be set as a VOCAB_PIXEL_RGB,
     * but the user can call the function with the pixel type of his/her choise. The convertion
     * if possible, will be done automatically on client side (TO BO VERIFIED).
     * Note: this will consume CPU power because it will not use GPU optimization.
     * Use VOCAB_PIXEL_RGB for best performances.
     *
     * @param rgbImage the image to be filled.
     * @param timeStamp time in which the image was acquired. Optional, the user must provide memory allocation
     * @return True on success
     */
    virtual bool getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp = NULL) = 0;

    /**
     * Get the depth frame from the device.
     * The pixel type of the source image will usually be set as a VOCAB_PIXEL_RGB,
     * but the user can call the function with the pixel type of his/her choise. The convertion
     * if possible, will be done automatically on client side.
     * Note: this will consume CPU power because it will not use GPU optimization.
     * Use VOCAB_PIXEL_RGB for best performances.
     *
     * @param rgbImage the image to be filled.
     * @param timeStamp time in which the image was acquired. Optional, the user must provide memory allocation
     * @return True on success
     */
    virtual bool getDepthImage(yarp::sig::FlexImage &depthImage, yarp::os::Stamp *timeStamp = NULL) = 0;

    /**
    * Get the both the color and depth frame in a single call. Implementation should assure the best possible synchronization
    * is achieved accordingly to synch policy set by the user.
    * TimeStamps are referred to acquisition time of the corresponding piece of information.
    * If the device is not providing TimeStamps, then 'timeStamp' field should be set to '-1'.
    * @param colorFrame pointer to FlexImage data to hold the color frame from the sensor
    * @param depthFrame pointer to FlexImage data to hold the depth frame from the sensor
    * @param colorStamp pointer to memory to hold the Stamp of the color frame
    * @param depthStamp pointer to memory to hold the Stamp of the depth frame
    * @return true if able to get both data.
    */
    virtual bool getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::FlexImage &depthFrame, yarp::os::Stamp *colorStamp=NULL, yarp::os::Stamp *depthStamp=NULL) = 0;

    /**
     * Get the surrent status of the sensor, using enum type
     *
     * @return an enum representing the status of the robot or an error code
     * if any error is present
     */
    virtual RGBDSensor_status getSensorStatus() = 0;
};

#endif   // YARP_DEV_RGBD_INTERFACE__
