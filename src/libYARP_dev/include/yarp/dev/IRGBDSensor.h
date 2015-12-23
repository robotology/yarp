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

    virtual ~IRGBDSensor(){}

    /** IDepthSensor Interface */
    using IDepthSensor::getDeviceInfo;
    using IDepthSensor::getMeasurementData;
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
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) = 0;
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) = 0;

    /** IRGBDSensor specific interface methods*/
    virtual bool getColor_and_Depth_Frames(yarp::sig::FlexImage colorFrame, yarp::sig::FlexImage depthFrame) = 0;
};

#endif   // __YARP_RGBD_INTERFACE__
