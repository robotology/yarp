/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "RGBDSensorClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


// needed for the driver factory.
yarp::dev::DriverCreator *createRGBDSensorClient() {
    return new DriverCreatorOf<yarp::dev::RGBDSensorClient>("RGBDSensorClient",
        "RGBDSensorClient",
        "yarp::dev::RGBDSensorClient");
}

RGBDSensorClient::RGBDSensorClient()
{
    watchdog = -1;
    sensor_p = NULL;
    use_ROS  = false;
    verbose  = 2;
    sensorStatus = IRGBDSensor::RGBD_SENSOR_NOT_READY;
}

RGBDSensorClient::~RGBDSensorClient()
{
    close();
}

bool RGBDSensorClient::open(yarp::os::Searchable& config)
{
    if(verbose >= 5)
        yTrace() << "\n Paramerters are: \n" << config.toString();

    if(!fromConfig(config))
    {
        yError() << "Device RGBDSensorClient failed to open, check previous log for error messsages.";
        return false;
    }

    sensorId= "RGBDSensorClient for " + local_depthFrame_StreamingPort_Name;

    if(!initialize_YARP(config) )
    {
        yError() << sensorId << "\n\t* Error initializing YARP ports *";
        return false;
    }

    if(!initialize_ROS(config) )
    {
        yError() << sensorId << "\n\t* Error initializing ROS topic *";
        return false;
    }
    return true;
}


bool RGBDSensorClient::fromConfig(yarp::os::Searchable &config)
{
    Bottle &rosGroup = config.findGroup("ROS");
    if(rosGroup.isNull())
    {
        if(verbose >= 3)
            yInfo() << "RGBDSensorClient: ROS configuration paramters are not set, skipping ROS topic initialization.";
        use_ROS  = false;
    }
    else
    {
        if(verbose >= 2)
            yWarning() << "RGBDSensorClient: ROS topic support is not yet implemented";
        use_ROS = false;
    }

    if(!use_ROS)  // default
    {
        // Parse LOCAL port names
        // TBD: check if user types '...' as port name, how to create RPC port names
        if (!config.check("localImagePort", "full name of the port for streaming color image"))
        {
            yError() << "RGBDSensorClient: missing 'localImagePort' parameter. Check you configuration file; it must be like:";
            yError() << "   localImagePort:         Full name of the local port to open, e.g. /myApp/image_camera";
            return false;
        }
        else
        {
            local_colorFrame_StreamingPort_Name  = config.find("localImagePort").asString().c_str();
            local_colorFrame_rpcPort_Name = local_colorFrame_StreamingPort_Name + "/rpc:i";
        }

        if (!config.check("localDepthPort", "full name of the port for streaming depth image"))
        {
            yError() << "RGBDSensorClient: missing 'localDepthPort' parameter. Check you configuration file; it must be like:";
            yError() << "   localDepthPort:         Full name of the local port to open, e.g. /myApp/depth_camera";
            return false;
        }
        else
        {
            local_depthFrame_StreamingPort_Name  = config.find("localDepthPort").asString().c_str();
            local_depthFrame_rpcPort_Name = local_depthFrame_StreamingPort_Name + "/rpc:i";
        }

        // Parse REMOTE port names
        if (!config.check("remoteImagePort", "full name of the port for streaming color image"))
        {
            yError() << "RGBDSensorClient: missing 'remoteImagePort' parameter. Check you configuration file; it must be like:";
            yError() << "   remoteImagePort:         Full name of the port to read color images from, e.g. /robotName/image_camera";
            return false;
        }
        else
        {
            remote_colorFrame_StreamingPort_Name  = config.find("remoteImagePort").asString().c_str();
            remote_colorFrame_rpcPort_Name = remote_colorFrame_StreamingPort_Name + "/rpc:i";
        }

        if (!config.check("remoteDepthPort", "full name of the port for streaming depth image"))
        {
            yError() << "RGBDSensorClient: missing 'remoteDepthPort' parameter. Check you configuration file; it must be like:";
            yError() << "   remoteDepthPort:         Full name of the port to read depth images from, e.g. /robotName/depth_camera ";
            return false;
        }
        else
        {
            remote_depthFrame_StreamingPort_Name  = config.find("remoteDepthPort").asString().c_str();
            remote_depthFrame_rpcPort_Name = remote_depthFrame_StreamingPort_Name + "/rpc:i";
        }
    }

    yarp::os::Bottle &ROS_parameters = config.findGroup("ROS");
    if(!ROS_parameters.isNull())
    {
        if(verbose >= 5)
            yInfo() << "RGBDSensorClient: found 'ROS' group in config file, parsing ROS specific parameters.";
        return false;
    }
    else
    {
        if(verbose >= 5)
            yInfo() << "RGBDSensorClient: 'ROS' group was NOT found in config file, skipping ROS specific parameters.";
    }

    if (!config.check("watchdog", "Verify refresh of data on ports whitin this time, otherwise throws an error"))
    {
        yError() << sensorId << "Missing 'watchdog' parameter. Check you configuration file; it must be like:";
        yError() << "   watchdog:    [ms]     Verify refresh of data on ports whitin this time, otherwise throws an error.";
        return false;
    }
    else
    {
        yWarning() << "Watchdog feature not yet implemented!!";
        watchdog = config.find("watchdog").asDouble();
    }


    return true;
}

bool RGBDSensorClient::initialize_YARP(yarp::os::Searchable &config)
{
    bool ret;
    // opening ports
    ret = colorFrame_StreamingPort.open(local_colorFrame_StreamingPort_Name.c_str());
    colorFrame_rpcPort.open(local_colorFrame_rpcPort_Name.c_str() );
//     colorFrame_rpcPort.setReader(RPC_parser);

    ret &=depthFrame_StreamingPort.open(local_depthFrame_StreamingPort_Name.c_str());
    depthFrame_rpcPort.open(local_depthFrame_rpcPort_Name.c_str() );
//     depthFrame_rpcPort.setReader(RPC_parser);

    if(!ret)
        yError() << "sensorId cannot open ports";

    // doing connections: How to correctly handle UARP_PORT_PREFIX for remote port names??
    if(! colorFrame_rpcPort.addOutput(remote_colorFrame_rpcPort_Name.c_str()) )  // This will handle local port names only
    {
        yError() << sensorId << " cannot add output " << remote_colorFrame_rpcPort_Name;
        return false;
    }

    if(! depthFrame_rpcPort.addOutput(remote_depthFrame_rpcPort_Name.c_str()) )  // This will handle local port names only
    {
        yError() << sensorId << " cannot add output " << remote_depthFrame_rpcPort_Name;
        return false;
    }

    if(! yarp::os::Network::connect(remote_colorFrame_StreamingPort_Name, colorFrame_StreamingPort.getName()) )
    {
        yError() << sensorId << " cannot connect to remote port " << remote_colorFrame_StreamingPort_Name;
        return false;
    }

    if(! yarp::os::Network::connect(remote_depthFrame_StreamingPort_Name, depthFrame_StreamingPort.getName()) )
    {
        yError() << sensorId << " cannot connect to remote port " << remote_depthFrame_StreamingPort_Name;
        return false;
    }

    streamingReader.attach(&colorFrame_StreamingPort, &depthFrame_StreamingPort);

    return true;
}

bool RGBDSensorClient::initialize_ROS(yarp::os::Searchable &config)
{
    if(use_ROS)
    {
        yError() << sensorId << "ROS topic is not supported yet";
        return false;
    }
    return true;
}

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
bool RGBDSensorClient::close()
{
    return true;
}

// IFrameGrabber Interfaces
/**
    * Get an rgb image from the frame grabber, if required
    * demosaicking/color reconstruction is applied
    *
    * @param image the image to be filled
    * @return true/false upon success/failure
    */
bool RGBDSensorClient::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> &image)
{
    return false;
}

/**
    * Get a raw image from the frame grabber
    *
    * @param image the image to be filled
    * @return true/false upon success/failure
    */
bool RGBDSensorClient::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono> &image)
{
    return false;
}

/**
    * Return the height of each frame.
    * @return image height
    */
int RGBDSensorClient::height() const
{
    return 0;
}

/**
    * Return the width of each frame.
    * @return image width
    */
int RGBDSensorClient::width() const
{
    return 0;
}


//
//  IDepth Interface
//
/**
* get the device hardware charactestics
* @param device_info Searchable struct containing the device info
* @return true if able to get information about the device.
*/
bool RGBDSensorClient::getDeviceInfo(yarp::os::Property &device_info)
{
    return false;
}

/**
* Get the distance measurements as an image
* @param ranges the vector containing the distance measurement
* @return true if able to get measurement data.
*/
bool RGBDSensorClient::getMeasurementData(yarp::sig::FlexImage &image, yarp::os::Stamp *stamp)
{
    return false;
}
/**
* get the device status
* @param status the device status
* @return true/false.
*/
bool RGBDSensorClient::getDeviceStatus(DepthSensor_status *status)
{
    return false;
}
/**
* get the device detection range
* @param min the minimum detection distance from the sensor [meter]
* @param max the maximum detection distance from the sensor [meter]
* @return true if able to get required info.
*/
bool RGBDSensorClient::getDistanceRange(double *min, double *max)
{
    return false;
}
/**
* set the device detection range. Invalid setting will be discarded.
* @param min the minimum detection distance from the sensor [meter]
* @param max the maximum detection distance from the sensor [meter]
* @return true if message was correctly delivered to the HW device.
*/
bool RGBDSensorClient::setDistanceRange(double min, double max)
{
    return false;
}
/**
* get the horizontal scan limits / field of view with respect to the
* front line of sight of the sensor. Angles are measured around the
* positive Z axis (counterclockwise, if Z is up) with zero angle being
* forward along the x axis
* @param min start angle of the scan  [degrees]
* @param max end angle of the scan    [degrees]
* @return true if able to get required info.
*/
bool RGBDSensorClient::getHorizontalScanLimits(double *min, double *max)
{
    return false;
}
/**
* set the horizontal scan limits / field of view with respect to the
* front line of sight of the sensor. Angles are measured around the
* positive Z axis (counterclockwise, if Z is up) with zero angle being
* forward along the x axis
* @param min start angle of the scan  [degrees]
* @param max end angle of the scan    [degrees]
* @return true if message was correctly delivered to the HW device.
*/
bool RGBDSensorClient::setHorizontalScanLimits(double min, double max)
{
    return false;
}
/**
* get the vertical scan limits / field of view with respect to the
* front line of sight of the sensor   [degrees]
* @param min start angle of the scan  [degrees]
* @param max end angle of the scan    [degrees]
* @return true if able to get required info.
*/
bool RGBDSensorClient::getVerticalScanLimits(double *min, double *max)
{
    return false;
}
/**
* set the vertical scan limits / field of view with respect to the
* front line of sight of the sensor   [degrees]
* @param min start angle of the scan  [degrees]
* @param max end angle of the scan    [degrees]
* @return true if message was correctly delivered to the HW device.
*/
bool RGBDSensorClient::setVerticalScanLimits(double min, double max)
{
    return false;
}
/**
* get the size of measured data from the device.
* It can be WxH for camera-like devices, or the number of points for other devices.
* @param horizontal width of image,  number of points in the horizontal scan [num]
* @param vertical   height of image, number of points in the vertical scan [num]
* @return true if able to get required info.
*/
bool RGBDSensorClient::getDataSize(double *horizontal, double *vertical)
{
    return false;
}
/**
* set the size of measured data from the device.
* It can be WxH for camera-like devices, or the number of points for other devices.
* @param horizontal width of image,  number of points in the horizontal scan [num]
* @param vertical   height of image, number of points in the vertical scan [num]
* @return true if message was correctly delivered to the HW device.
*/
bool RGBDSensorClient::setDataSize(double horizontal, double vertical)
{
    return false;
}
/**
* get the device resolution, using the current settings of scan limits
* and data size. Will return the resolution of device at 1 meter distance.
* @param hRes horizontal resolution [meter]
* @param vRes vertical resolution [meter]
* @return true if able to get required info.
*/
bool RGBDSensorClient::getResolution(double *hRes, double *vRes)
{
    return false;
}
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
bool RGBDSensorClient::setResolution(double hRes, double vRes)
{
    return false;
}
/**
* get the scan rate (scans per seconds)
* @param rate the scan rate
* @return true if able to get required info.
*/
bool RGBDSensorClient::getScanRate(double *rate)
{
    return false;
}
/**
* set the scan rate (scans per seconds)
* @param rate the scan rate
* @return true if message was correctly delivered to the HW device.
*/
bool RGBDSensorClient::setScanRate(double rate)
{
    return false;
}

/** IRGBDSensor specific interface methods*/
bool RGBDSensorClient::getRGBDSensor_Status(RGBDSensor_status *status)
{
    return false;
}

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
bool RGBDSensorClient::getRGBD_Frames(yarp::sig::FlexImage &colorFrame, yarp::sig::FlexImage &depthFrame, yarp::os::Stamp *colorStamp, yarp::os::Stamp *depthStamp)
{
    streamingReader.synchRead(colorFrame, depthFrame);
    return true;
}

