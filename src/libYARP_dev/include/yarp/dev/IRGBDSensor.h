// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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


#ifndef __YARP_RGBD_INTERFACE__
#define __YARP_RGBD_INTERFACE__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/Image.h>
#include <yarp/os/Stamp.h>

#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/IDepthSensor.h>

namespace yarp {
    namespace dev {
        class IRGBDSensor;
    }
}

/**
 * @ingroup dev_iface_other
 *
 * A generic interface for cameras that have both color camera as well
 * as depth camera sensor, like kinect device.
 *
 * This interface extends the existing FrameGrabber and IDepthSensor
 * interfaces by adding a method to get both images at the same time
 * in sych and allowing the client to set a synch policy.
 *
 * How the synch policy is implemented depends on the client device.
 */

class YARP_dev_API yarp::dev::IRGBDSensor:  public IFrameGrabberImage,
                                            public IFrameGrabberImageRaw,
                                            public IDepthSensor
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
        RGBD_SENSOR_GENERAL_ERROR    = 3,
        RGBD_SENSOR_TIMEOUT          = 4
    };

    virtual ~IRGBDSensor(){}

    /** IDepthSensor Interface */
    using IDepthSensor::getDeviceInfo;
    using IDepthSensor::getMeasurementData;
    using IDepthSensor::getDeviceStatus;
    using IDepthSensor::getDistanceRange;
    using IDepthSensor::setDistanceRange;
    using IDepthSensor::getHorizontalScanLimits;
    using IDepthSensor::setHorizontalScanLimits;
    using IDepthSensor::getverticalScanLimits;
    using IDepthSensor::setverticalScanLimits;
    using IDepthSensor::getDataSize;
    using IDepthSensor::setDataSize;
    using IDepthSensor::getResolution;
    using IDepthSensor::setResolution;
    using IDepthSensor::getScanRate;
    using IDepthSensor::setScanRate;

    /** IFrameGrabberImage interface */
    virtual int height() const = 0;
    virtual int width() const = 0;
//     using IFrameGrabberImage::getImage;
//     using IFrameGrabberImageRaw::getImage;
//     virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)  = 0;
//     virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) = 0;

    /** IRGBDSensor specific interface methods*/
    virtual bool getRGBDSensor_Status(RGBDSensor_status *status) = 0;

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
    virtual bool getRGBD_Frames(yarp::sig::FlexImage &colorFrame, yarp::sig::FlexImage &depthFrame, yarp::os::Stamp *colorStamp=NULL, yarp::os::Stamp *depthStamp=NULL) = 0;
};

#endif   // __YARP_RGBD_INTERFACE__
