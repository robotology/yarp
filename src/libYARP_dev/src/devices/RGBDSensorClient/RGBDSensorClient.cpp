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

bool RGBDSensorClient::close()
{
    return true;
}


int  RGBDSensorClient::getRgbHeight()
{
    return false;
}

int  RGBDSensorClient::getRgbWidth()
{
    return false;
}

bool RGBDSensorClient::getRgbFOV(int &horizontalFov, int &verticalFov)
{
    return false;
}

bool RGBDSensorClient::getRgbIntrinsicParam(yarp::os::Property &intrinsic)
{
    return false;
}

bool RGBDSensorClient::getRgbSensorInfo(yarp::os::Property &info)
{
    return false;
}


    /*
    * IDepthVisualParams interface. Look at IVisualParams.h for documentation
    */
int    RGBDSensorClient::getDepthHeight()
{
    return false;
}

int    RGBDSensorClient::getDepthWidth()
{
    return false;
}

bool   RGBDSensorClient::getDepthFOV(int &horizontalFov, int &verticalFov)
{
    return false;
}

bool   RGBDSensorClient::getDepthIntrinsicParam(yarp::os::Property &intrinsic)
{
    return false;
}

bool   RGBDSensorClient::getDepthSensorInfo(yarp::os::Property info)
{
    return false;
}

double RGBDSensorClient::getDepthAccuracy()
{
    return false;
}

bool   RGBDSensorClient::getDepthClipPlanes(int &near, int &far)
{
    return false;
}

bool RGBDSensorClient::setDepthClipPlanes(int near, int far)
{
    return false;
}


/** IRGBDSensor specific interface methods*/

bool RGBDSensorClient::getExtrinsicParam(yarp::os::Property &extrinsic)
{
    return false;
}


IRGBDSensor::RGBDSensor_status RGBDSensorClient::getSensorStatus()
{
    return RGBD_SENSOR_NOT_READY;
}


yarp::os::ConstString RGBDSensorClient::getLastErrorMsg(yarp::os::Stamp *timeStamp)
{
    return "false";
}


bool RGBDSensorClient::getRgbImage(yarp::sig::FlexImage   &rgbImage,   yarp::os::Stamp *timeStamp)
{
    return false;
}


bool RGBDSensorClient::getDepthImage(yarp::sig::FlexImage &depthImage, yarp::os::Stamp *timeStamp)
{
    return false;
}


bool RGBDSensorClient::getSynchPolicy(SynchPolicy policy, yarp::os::Property params)
{
    return false;
}


bool RGBDSensorClient::getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::FlexImage &depthFrame, yarp::os::Stamp *colorStamp, yarp::os::Stamp *depthStamp)
{
    return false;
}


