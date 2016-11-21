/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Portable.h>
#include <yarp/os/LogStream.h>
#include "RGBDSensorClient.h"

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


// needed for the driver factory.
yarp::dev::DriverCreator *createRGBDSensorClient()
{
    return new DriverCreatorOf<yarp::dev::RGBDSensorClient>("RGBDSensorClient",
        "RGBDSensorClient",
        "yarp::dev::RGBDSensorClient");
}

RGBDSensorClient::RGBDSensorClient() :  Implement_RgbVisualParams_Sender(rpcPort),
                                        Implement_DepthVisualParams_Sender(rpcPort)
{
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

    sensorId= "RGBDSensorClient for " + local_depthFrame_StreamingPort_name;

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
            local_colorFrame_StreamingPort_name  = config.find("localImagePort").asString().c_str();
        }

        if (!config.check("localDepthPort", "full name of the port for streaming depth image"))
        {
            yError() << "RGBDSensorClient: missing 'localDepthPort' parameter. Check you configuration file; it must be like:";
            yError() << "   localDepthPort:         Full name of the local port to open, e.g. /myApp/depth_camera";
            return false;
        }
        else
        {
            local_depthFrame_StreamingPort_name  = config.find("localDepthPort").asString().c_str();
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
            remote_colorFrame_StreamingPort_name  = config.find("remoteImagePort").asString().c_str();
        }

        if (!config.check("remoteDepthPort", "full name of the port for streaming depth image"))
        {
            yError() << "RGBDSensorClient: missing 'remoteDepthPort' parameter. Check you configuration file; it must be like:";
            yError() << "   remoteDepthPort:         Full name of the port to read depth images from, e.g. /robotName/depth_camera ";
            return false;
        }
        else
        {
            remote_depthFrame_StreamingPort_name  = config.find("remoteDepthPort").asString().c_str();
        }

        // Single RPC port
        if (!config.check("localRpcPort", "full name of the port for streaming depth image"))
        {
            yError() << "RGBDSensorClient: missing 'localRpcPort' parameter. Check you configuration file; it must be like:";
            yError() << "   localRpcPort:            Full name of the local RPC port to open, e.g. /myApp/RGBD/rpc";
            return false;
        }
        else
        {
            local_rpcPort_name  = config.find("localRpcPort").asString().c_str();
        }

        if (!config.check("remoteRpcPort", "full name of the port for streaming depth image"))
        {
            yError() << "RGBDSensorClient: missing 'remoteRpcPort' parameter. Check you configuration file; it must be like:";
            yError() << "   remoteRpcPort:         Full name of the remote RPC port, e.g. /robotName/RGBD/rpc";
            return false;
        }
        else
        {
            remote_rpcPort_name  = config.find("remoteRpcPort").asString().c_str();
        }


        /*
            * When using multiple RPC ports
            *
             local_colorFrame_rpcPort_Name =  local_colorFrame_StreamingPort_Name + "/rpc:i";
            remote_colorFrame_rpcPort_Name = remote_colorFrame_StreamingPort_Name + "/rpc:i";
             local_depthFrame_rpcPort_Name =  local_depthFrame_StreamingPort_Name + "/rpc:i";
            remote_depthFrame_rpcPort_Name = remote_depthFrame_StreamingPort_Name + "/rpc:i";

        */
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

    return true;
}

bool RGBDSensorClient::initialize_YARP(yarp::os::Searchable &config)
{
    bool ret;

    // Opening Streaming ports
    ret  = colorFrame_StreamingPort.open(local_colorFrame_StreamingPort_name.c_str());
    ret &= depthFrame_StreamingPort.open(local_depthFrame_StreamingPort_name.c_str());

    if(!ret)
    {
        yError() << sensorId << " cannot open local streaming ports.";
        colorFrame_StreamingPort.close();
        depthFrame_StreamingPort.close();
    }

    if(! yarp::os::Network::connect(remote_colorFrame_StreamingPort_name, colorFrame_StreamingPort.getName()), "udp")
    {
        yError() << sensorId << " cannot connect to remote port " << remote_colorFrame_StreamingPort_name;
        return false;
    }

    if(! yarp::os::Network::connect(remote_depthFrame_StreamingPort_name, depthFrame_StreamingPort.getName()), "udp")
    {
        yError() << sensorId << " cannot connect to remote port " << remote_depthFrame_StreamingPort_name;
        return false;
    }


    // Single RPC port
    ret = rpcPort.open(local_rpcPort_name);

    if(!ret)
    {
        yError() << sensorId << " cannot open local RPC port " << local_rpcPort_name;
        colorFrame_StreamingPort.close();
        depthFrame_StreamingPort.close();
        rpcPort.close();
    }

    if(! rpcPort.addOutput(remote_rpcPort_name.c_str()) )
    {
        yError() << sensorId << " cannot connect to port " << remote_rpcPort_name;
        colorFrame_StreamingPort.close();
        depthFrame_StreamingPort.close();
        rpcPort.close();
        return false;
    }

   /*
    * Multiple RPC ports
    *
    ret &= colorFrame_rpcPort.open(local_colorFrame_rpcPort_Name.c_str() );
    ret &= depthFrame_rpcPort.open(local_depthFrame_rpcPort_Name.c_str() );

    if(!ret)
        yError() << "sensorId cannot open ports";

    // doing connections: How to correctly handle YARP_PORT_PREFIX for remote port names??
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
    */


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

/*
 * IDepthVisualParams interface. Look at IVisualParams.h for documentation
 *
 * Implemented by Implement_DepthVisualParams_Sender
 */

/*
 * IDepthVisualParams interface. Look at IVisualParams.h for documentation
 *
 * Implemented by Implement_DepthVisualParams_Sender
 */


/*
 * IRGBDSensor specific interface methods
 */

bool RGBDSensorClient::getExtrinsicParam(yarp::os::Property &extrinsic)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGBD_SENSOR);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_EXTRINSIC_PARAM);
    rpcPort.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if((response.get(0).asVocab()) == VOCAB_FAILED)
    {
        extrinsic.clear();
        return false;
    }

    return Property::copyPortable(response.get(3), extrinsic);  // will it really work??
}


IRGBDSensor::RGBDSensor_status RGBDSensorClient::getSensorStatus()
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGBD_SENSOR);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_STATUS);
    rpcPort.write(cmd, response);
    return (IRGBDSensor::RGBDSensor_status) response.get(3).asInt();
}


yarp::os::ConstString RGBDSensorClient::getLastErrorMsg(yarp::os::Stamp *timeStamp)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGBD_SENSOR);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ERROR_MSG);
    rpcPort.write(cmd, response);
    return response.get(3).asString();
}

bool RGBDSensorClient::getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp)
{
    streamingReader.readRgb(rgbImage);
    if(timeStamp)
        timeStamp->update(yarp::os::Time::now());
    return false;
}

bool RGBDSensorClient::getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp)
{
    return false;
}

bool RGBDSensorClient::getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthFrame, yarp::os::Stamp *colorStamp, yarp::os::Stamp *depthStamp)
{
    return false;
}

// IFrame Grabber Control 2
bool RGBDSensorClient::getCameraDescription(CameraDescriptor *camera)
{
    return false;
}

bool RGBDSensorClient::hasFeature(int feature, bool *hasFeature)
{
    return false;
}

bool RGBDSensorClient::setFeature(int feature, double value)
{
    return false;
}

bool RGBDSensorClient::getFeature(int feature, double *value)
{
    return false;
}

bool RGBDSensorClient::setFeature(int feature, double value1, double value2)
{
    return false;
}

bool RGBDSensorClient::getFeature(int feature, double *value1, double *value2)
{
    return false;
}

bool RGBDSensorClient::hasOnOff(  int feature, bool *HasOnOff)
{
    return false;
}

bool RGBDSensorClient::setActive( int feature, bool onoff)
{
    return false;
}

bool RGBDSensorClient::getActive( int feature, bool *isActive)
{
    return false;
}

bool RGBDSensorClient::hasAuto(   int feature, bool *hasAuto)
{
    return false;
}

bool RGBDSensorClient::hasManual( int feature, bool *hasManual)
{
    return false;
}

bool RGBDSensorClient::hasOnePush(int feature, bool *hasOnePush)
{
    return false;
}

bool RGBDSensorClient::setMode(   int feature, FeatureMode mode)
{
    return false;
}

bool RGBDSensorClient::getMode(   int feature, FeatureMode *mode)
{
    return false;
}

bool RGBDSensorClient::setOnePush(int feature)
{
    return false;
}
