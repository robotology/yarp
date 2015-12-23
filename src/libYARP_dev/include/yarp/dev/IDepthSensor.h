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

#ifndef __YARP_DEPTHSENSOR_INTERFACE__
#define __YARP_DEPTHSENSOR_INTERFACE__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/Image.h>

#define VOCAB_IDEPTH_SENSOR         VOCAB4('i','d','p','t')
#define VOCAB_SENSOR_INFO           VOCAB4('i','n','f','o')
#define VOCAB_SENSOR_STATUS         VOCAB4('s','t','a','t')
#define VOCAB_DISTANCE_RANGE        VOCAB4('r','a','n','g')
#define VOCAB_HSCAN_LIMIT           VOCAB4('h','l','i','m')
#define VOCAB_VSCAN_LIMIT           VOCAB4('v','l','i','m')
#define VOCAB_DATA_SIZE             VOCAB4('s','i','z','e')
#define VOCAB_HV_RESOLUTION         VOCAB3('r','e','s')
#define VOCAB_SCAN_RATE             VOCAB4('r','a','t','e')

namespace yarp {
    namespace dev {
        class IDepthSensor;
    }
}

/**
 * @ingroup dev_iface_other
 *
 * A generic interface for depth sensor device
 * This implementation is targeted for depth cameras like
 * kinect devices.
 * For linear scanner, the IRangefinder2D interface shall be used instead.
 *
 * This interface could be extended in the future to take
 * into account other devices like planar lasers or single point
 * range measurement devices like ultrasound systems, infrared proximity
 * sensors and so on, but not yet at this time.
 *
 * Laser or light devices can easily fit into this interface while
 * sonar devices doesn't, so a different interface/device shall be used instead.
 */

class YARP_dev_API yarp::dev::IDepthSensor
{
public:
    enum DepthSensor_status
    {
        DEPTHSENSOR_NOT_READY        = 0,
        DEPTHSENSOR_OK_STANBY        = 1,
        DEPTHSENSOR_OK_IN_USE        = 2,
        DEPTHSENSOR_GENERAL_ERROR    = 3,
        DEPTHSENSOR_TIMEOUT          = 4
    };

    virtual ~IDepthSensor(){}

   /**
    * get the device hardware charactestics
    * @param device_info Searchable struct containing the device info
    * @return true if able to get information about the device.
    */
    virtual bool getDeviceInfo(yarp::os::Searchable &device_info) = 0;

   /**
    * Get the distance measurements as a Matrix
    * @param ranges the vector containing the distance measurement
    * @return true if able to get measurement data.
    */
    virtual bool getMeasurementData(yarp::sig::Matrix &data) = 0;

   /**
    * Get the distance measurements as an image
    * @param ranges the vector containing the distance measurement
    * @return true if able to get measurement data.
    */
    virtual bool getMeasurementData(yarp::sig::FlexImage &image) = 0;

   /**
    * get the device status
    * @param status the device status
    * @return true/false.
    */
    virtual bool getDeviceStatus(DepthSensor_status& status) = 0;

   /**
    * get the device detection range
    * @param min the minimum detection distance from the sensor [meter]
    * @param max the maximum detection distance from the sensor [meter]
    * @return true if able to get required info.
    */
    virtual bool getDistanceRange(double& min, double& max) = 0;

   /**
    * set the device detection range. Invalid setting will be discarded.
    * @param min the minimum detection distance from the sensor [meter]
    * @param max the maximum detection distance from the sensor [meter]
    * @return true if message was correctly delivered to the HW device.
    */
    virtual bool setDistanceRange(double min, double max) = 0;

   /**
    * get the horizontal scan limits / field of view with respect to the
    * front line of sight of the sensor. Angles are measured around the
    * positive Z axis (counterclockwise, if Z is up) with zero angle being
    * forward along the x axis
    * @param min start angle of the scan  [degrees]
    * @param max end angle of the scan    [degrees]
    * @return true if able to get required info.
    */
    virtual bool getHorizontalScanLimits(double& min, double& max) = 0;

   /**
    * set the horizontal scan limits / field of view with respect to the
    * front line of sight of the sensor. Angles are measured around the
    * positive Z axis (counterclockwise, if Z is up) with zero angle being
    * forward along the x axis
    * @param min start angle of the scan  [degrees]
    * @param max end angle of the scan    [degrees]
    * @return true if message was correctly delivered to the HW device.
    */
    virtual bool setHorizontalScanLimits(double min, double max) = 0;

   /**
    * get the vertical scan limits / field of view with respect to the
    * front line of sight of the sensor   [degrees]
    * @param min start angle of the scan  [degrees]
    * @param max end angle of the scan    [degrees]
    * @return true if able to get required info.
    */
    virtual bool getverticalScanLimits(double& min, double& max) = 0;

   /**
    * set the vertical scan limits / field of view with respect to the
    * front line of sight of the sensor   [degrees]
    * @param min start angle of the scan  [degrees]
    * @param max end angle of the scan    [degrees]
    * @return true if message was correctly delivered to the HW device.
    */
    virtual bool setverticalScanLimits(double min, double max) = 0;

   /**
    * get the size of measured data from the device.
    * It can be WxH for camera-like devices, or the number of points for other devices.
    * @param horizontal width of image,  number of points in the horizontal scan [num]
    * @param vertical   height of image, number of points in the vertical scan [num]
    * @return true if able to get required info.
    */
    virtual bool getDataSize(double& horizontal, double &vertical) = 0;

   /**
    * set the size of measured data from the device.
    * It can be WxH for camera-like devices, or the number of points for other devices.
    * @param horizontal width of image,  number of points in the horizontal scan [num]
    * @param vertical   height of image, number of points in the vertical scan [num]
    * @return true if message was correctly delivered to the HW device.
    */
    virtual bool setDataSize(double& horizontal, double &vertical) = 0;

   /**
    * get the device resolution, using the current settings of scan limits
    * and data size. Will return the resolution of device at 1 meter distance.
    * @param hRes horizontal resolution [meter]
    * @param vRes vertical resolution [meter]
    * @return true if able to get required info.
    */
    virtual bool getResolution(double& hRes, double &vRes) = 0;

   /**
    * set the device resolution.
    * This call can change the current settings of scan limits, data size or scan rate
    * to match the requested resolution.
    * Verify those settings is suggested after this call.
    * Will set the resolution of device at 1meter distance, if possible.
    * @param hRes horizontal resolution [meter]
    * @param vRes vertical resolution [meter]
    * @return true if message was correctly delivered to the HW device.
    */
    virtual bool setResolution(double& hRes, double &vRes) = 0;

   /**
    * get the scan rate (scans per seconds)
    * @param rate the scan rate
    * @return true if able to get required info.
    */
    virtual bool getScanRate(double& rate) = 0;

   /**
    * set the scan rate (scans per seconds)
    * @param rate the scan rate
    * @return true if message was correctly delivered to the HW device.
    */
    virtual bool setScanRate(double rate) = 0;
};

#endif   // __YARP_DEPTHSENSOR_INTERFACE__
