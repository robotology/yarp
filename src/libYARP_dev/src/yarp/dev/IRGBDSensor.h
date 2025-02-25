/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IRGBDSENSOR_H
#define YARP_DEV_IRGBDSENSOR_H

#include <yarp/os/Stamp.h>
#include <yarp/os/Property.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Matrix.h>
#include <yarp/dev/api.h>
#include <yarp/dev/IRgbVisualParams.h>
#include <yarp/dev/IDepthVisualParams.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {

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

class YARP_dev_API IRGBDSensor :
        public IRgbVisualParams,
        public IDepthVisualParams
{
public:

    // We should distinguish between rgb and depth sensors, which one is working
    // and which one isn't. Maybe a specific function with two separated values is better.
    // Here values are referred to the sensor as a whole.
    enum RGBDSensor_status
    {
        RGBD_SENSOR_NOT_READY        = 0,
        RGBD_SENSOR_OK_STANDBY       = 1,
        RGBD_SENSOR_OK_IN_USE        = 2,
        RGB_SENSOR_ERROR             = 3,
        DEPTH_SENSOR_ERROR           = 4,
        RGBD_SENSOR_GENERIC_ERROR    = 5,
        RGBD_SENSOR_TIMEOUT          = 6
    };

    virtual ~IRGBDSensor();

    /*
     * IRGBDSensor specific interface methods
     */

    /**
     * Get the extrinsic parameters from the device
     * @param  extrinsic  return a rototranslation matrix describing the position
     *         of the depth optical frame with respect to the rgb frame
     * @return true if success
     */
    virtual yarp::dev::ReturnValue getExtrinsicParam(yarp::sig::Matrix &extrinsic) = 0;

    /**
     * Return an error message in case of error. For debugging purpose and user notification.
     * Error message will be reset after any successful command
     * @param message A string explaining the last error occurred.
     * @return True on success
     */
    virtual yarp::dev::ReturnValue getLastErrorMsg(std::string& message, yarp::os::Stamp *timeStamp = nullptr) = 0;

    /**
     * Get the rgb frame from the device.
     * The pixel type of the source image will usually be set as a VOCAB_PIXEL_RGB,
     * but the user can call the function with the pixel type of his/her choice. The conversion
     * if possible, will be done automatically on client side (TO BO VERIFIED).
     * Note: this will consume CPU power because it will not use GPU optimization.
     * Use VOCAB_PIXEL_RGB for best performances.
     *
     * @param rgbImage the image to be filled.
     * @param timeStamp time in which the image was acquired. Optional, ignored if nullptr.
     * @return True on success
     */
    virtual yarp::dev::ReturnValue getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp = nullptr) = 0;

    /**
     * Get the depth frame from the device.
     *
     * @param depthImage the depth image to be filled, depth measured in meters.
     * @param timeStamp time in which the image was acquired. Optional, ignored if nullptr.
     * @return True on success
     */
    virtual yarp::dev::ReturnValue getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp = nullptr) = 0;

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
    virtual yarp::dev::ReturnValue getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthFrame, yarp::os::Stamp *colorStamp=nullptr, yarp::os::Stamp *depthStamp=nullptr) = 0;

    /**
     * Get the current status of the sensor, using enum type
     *
     * @param status an enum representing the status of the robot or an error code
     * if any error is present
     * @return True on success
     */
    virtual yarp::dev::ReturnValue getSensorStatus(RGBDSensor_status& status) = 0;
};

} // namespace yarp::dev

#endif   // YARP_DEV_IRGBDSENSOR_H
