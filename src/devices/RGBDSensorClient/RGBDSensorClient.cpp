/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "RGBDSensorClient.h"
#include "RGBDSensorClient_StreamingMsgParser.h"
#include <yarp/os/Portable.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/GenericVocabs.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

#define RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR 1
#define RGBD_INTERFACE_PROTOCOL_VERSION_MINOR 0

RGBDSensorClient::RGBDSensorClient() :
        FrameGrabberControls_Sender(rpcPort),
        RgbMsgSender(new Implement_RgbVisualParams_Sender(rpcPort)),
        DepthMsgSender(new Implement_DepthVisualParams_Sender(rpcPort)),
        streamingReader(new RGBDSensor_StreamingMsgParser)
{
}

RGBDSensorClient::~RGBDSensorClient()
{
    close();
    delete RgbMsgSender;
    delete DepthMsgSender;
    delete streamingReader;
}

bool RGBDSensorClient::open(yarp::os::Searchable& config)
{
    if(verbose >= 5)
        yTrace() << "\n Paramerters are: \n" << config.toString();

    if(!fromConfig(config))
    {
        yError() << "Device RGBDSensorClient failed to open, check previous log for error messages.";
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
            yInfo() << "RGBDSensorClient: ROS configuration parameters are not set, skipping ROS topic initialization.";
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
            local_colorFrame_StreamingPort_name  = config.find("localImagePort").asString();
        }

        if (!config.check("localDepthPort", "full name of the port for streaming depth image"))
        {
            yError() << "RGBDSensorClient: missing 'localDepthPort' parameter. Check you configuration file; it must be like:";
            yError() << "   localDepthPort:         Full name of the local port to open, e.g. /myApp/depth_camera";
            return false;
        }
        else
        {
            local_depthFrame_StreamingPort_name  = config.find("localDepthPort").asString();
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
            remote_colorFrame_StreamingPort_name  = config.find("remoteImagePort").asString();
        }

        if (!config.check("remoteDepthPort", "full name of the port for streaming depth image"))
        {
            yError() << "RGBDSensorClient: missing 'remoteDepthPort' parameter. Check you configuration file; it must be like:";
            yError() << "   remoteDepthPort:         Full name of the port to read depth images from, e.g. /robotName/depth_camera ";
            return false;
        }
        else
        {
            remote_depthFrame_StreamingPort_name  = config.find("remoteDepthPort").asString();
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
            local_rpcPort_name  = config.find("localRpcPort").asString();
        }

        if (!config.check("remoteRpcPort", "full name of the port for streaming depth image"))
        {
            yError() << "RGBDSensorClient: missing 'remoteRpcPort' parameter. Check you configuration file; it must be like:";
            yError() << "   remoteRpcPort:         Full name of the remote RPC port, e.g. /robotName/RGBD/rpc";
            return false;
        }
        else
        {
            remote_rpcPort_name  = config.find("remoteRpcPort").asString();
        }

        image_carrier_type = "udp";
        depth_carrier_type = "udp";

        if (config.check("ImageCarrier", "carrier for the image stream"))
        {
            image_carrier_type = config.find("ImageCarrier").asString();
        }

        if (config.check("DepthCarrier", "carrier for the depth tream"))
        {
            depth_carrier_type = config.find("DepthCarrier").asString();
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
    ret  = colorFrame_StreamingPort.open(local_colorFrame_StreamingPort_name);
    ret &= depthFrame_StreamingPort.open(local_depthFrame_StreamingPort_name);

    if(!ret)
    {
        yError() << sensorId << " cannot open local streaming ports.";
        colorFrame_StreamingPort.close();
        depthFrame_StreamingPort.close();
    }

    if(! yarp::os::Network::connect(remote_colorFrame_StreamingPort_name, colorFrame_StreamingPort.getName(), image_carrier_type) )
    {
        yError() << colorFrame_StreamingPort.getName() << " cannot connect to remote port " << remote_colorFrame_StreamingPort_name << "with carrier " << image_carrier_type;
        return false;
    }

    if(! yarp::os::Network::connect(remote_depthFrame_StreamingPort_name, depthFrame_StreamingPort.getName(), depth_carrier_type) )
    {
        yError() << depthFrame_StreamingPort.getName() << " cannot connect to remote port " << remote_depthFrame_StreamingPort_name << "with carrier " << depth_carrier_type;
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

    if(! rpcPort.addOutput(remote_rpcPort_name) )
    {
        yError() << sensorId << " cannot connect to port " << remote_rpcPort_name;
        colorFrame_StreamingPort.close();
        depthFrame_StreamingPort.close();
        rpcPort.close();
        return false;
    }

    // Check protocol version
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGBD_SENSOR);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_RGBD_PROTOCOL_VERSION);
    rpcPort.write(cmd, response);
    int major = response.get(3).asInt32();
    int minor = response.get(4).asInt32();

    if(major != RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR)
    {
        yError() << "Major protocol number does not match, please verify client and server are updated. \
                    Expected: " << RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR << " received: " << major;
        return false;
    }


    if(minor != RGBD_INTERFACE_PROTOCOL_VERSION_MINOR)
    {
        yWarning() << "Minor protocol number does not match, please verify client and server are updated.\
                      Expected: " << RGBD_INTERFACE_PROTOCOL_VERSION_MINOR << " received: " << minor;
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


    streamingReader->attach(&colorFrame_StreamingPort, &depthFrame_StreamingPort);

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
    colorFrame_StreamingPort.close();
    depthFrame_StreamingPort.close();
    rpcPort.close();
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

bool RGBDSensorClient::getExtrinsicParam(yarp::sig::Matrix &extrinsic)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_RGBD_SENSOR);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_EXTRINSIC_PARAM);
    rpcPort.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if((response.get(0).asVocab()) == VOCAB_FAILED)
    {
        extrinsic.zero();
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
    return (IRGBDSensor::RGBDSensor_status) response.get(3).asInt32();
}


std::string RGBDSensorClient::getLastErrorMsg(yarp::os::Stamp *timeStamp)
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
    if(timeStamp)
        timeStamp->update(yarp::os::Time::now());
    return streamingReader->readRgb(rgbImage);
}

bool RGBDSensorClient::getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp)
{
    if(timeStamp)
        timeStamp->update(yarp::os::Time::now());
    return streamingReader->readDepth(depthImage);
}

bool RGBDSensorClient::getImages(FlexImage &rgbImage, ImageOf<PixelFloat> &depthImage, Stamp *rgbStamp, Stamp *depthStamp)
{
    bool ret = true;
    ret &= streamingReader->readRgb(rgbImage);
    ret &= streamingReader->readDepth(depthImage);

    if(rgbStamp)
        rgbStamp->update(yarp::os::Time::now());

    if(depthStamp)
        depthStamp->update(yarp::os::Time::now());
    return ret;
}

//
// IFrame Grabber Control 2 interface is implemented by FrameGrabberControls2_Sender
//

//
// Rgb
//
int RGBDSensorClient::getRgbHeight()
{
    return RgbMsgSender->getRgbHeight();
}

int RGBDSensorClient::getRgbWidth()
{
    return RgbMsgSender->getRgbWidth();
}
bool RGBDSensorClient::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations)
{
    return RgbMsgSender->getRgbSupportedConfigurations(configurations);
}
bool RGBDSensorClient::getRgbResolution(int &width, int &height){
    return RgbMsgSender->getRgbResolution(width, height);
}

bool RGBDSensorClient::setRgbResolution(int width, int height)
{
    return RgbMsgSender->setRgbResolution(width, height);
}
bool RGBDSensorClient::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    return RgbMsgSender->getRgbFOV(horizontalFov, verticalFov);
}
bool RGBDSensorClient::setRgbFOV(double horizontalFov, double verticalFov)
{
    return RgbMsgSender->getRgbFOV(horizontalFov, verticalFov);
}
bool RGBDSensorClient::getRgbIntrinsicParam(yarp::os::Property &intrinsic)
{
    return RgbMsgSender->getRgbIntrinsicParam(intrinsic);
}

bool RGBDSensorClient::getRgbMirroring(bool& mirror)
{
    return RgbMsgSender->getRgbMirroring(mirror);
}

bool RGBDSensorClient::setRgbMirroring(bool mirror)
{
    return RgbMsgSender->setRgbMirroring(mirror);
}

//
// Depth
//
int RGBDSensorClient::getDepthHeight()
{
    return DepthMsgSender->getDepthHeight();
}
int RGBDSensorClient::getDepthWidth()
{
    return DepthMsgSender->getDepthWidth();
}
bool RGBDSensorClient::setDepthResolution(int width, int height)
{
    return DepthMsgSender->setDepthResolution(width, height);
}
bool RGBDSensorClient::getDepthFOV(double &horizontalFov, double &verticalFov)
{
    return DepthMsgSender->getDepthFOV(horizontalFov, verticalFov);
}
bool RGBDSensorClient::setDepthFOV(double horizontalFov, double verticalFov)
{
    return DepthMsgSender->setDepthFOV(horizontalFov, verticalFov);
}
double RGBDSensorClient::getDepthAccuracy()
{
    return DepthMsgSender->getDepthAccuracy();
}
bool RGBDSensorClient::setDepthAccuracy(double accuracy)
{
    return DepthMsgSender->setDepthAccuracy(accuracy);
}
bool RGBDSensorClient::getDepthClipPlanes(double &nearPlane, double &farPlane)
{
    return DepthMsgSender->getDepthClipPlanes(nearPlane, farPlane);
}
bool RGBDSensorClient::setDepthClipPlanes(double nearPlane, double farPlane)
{
    return DepthMsgSender->setDepthClipPlanes(nearPlane, farPlane);
}
bool RGBDSensorClient::getDepthIntrinsicParam(yarp::os::Property &intrinsic)
{
    return DepthMsgSender->getDepthIntrinsicParam(intrinsic);
}

bool RGBDSensorClient::getDepthMirroring(bool& mirror)
{
    return DepthMsgSender->getDepthMirroring(mirror);
}

bool RGBDSensorClient::setDepthMirroring(bool mirror)
{
    return DepthMsgSender->setDepthMirroring(mirror);
}
