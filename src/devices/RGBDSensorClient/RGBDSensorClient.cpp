/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RGBDSensorClient.h"
#include "RGBDSensorClient_StreamingMsgParser.h"
#include <yarp/os/Portable.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/proto/framegrabber/CameraVocabs.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

#define RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR 1
#define RGBD_INTERFACE_PROTOCOL_VERSION_MINOR 0

YARP_LOG_COMPONENT(RGBDSENSORCLIENT, "yarp.devices.RGBDSensorClient")


RGBDSensorClient::RGBDSensorClient() :
        yarp::proto::framegrabber::FrameGrabberControls_Forwarder(rpcPort),
        RgbMsgSender(new yarp::proto::framegrabber::RgbVisualParams_Forwarder(rpcPort)),
        DepthMsgSender(new yarp::proto::framegrabber::DepthVisualParams_Forwarder(rpcPort)),
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
    if(verbose >= 5) {
        yCTrace(RGBDSENSORCLIENT) << "\n Paramerters are: \n" << config.toString();
    }

    if(!fromConfig(config))
    {
        yCError(RGBDSENSORCLIENT) << "Failed to open, check previous log for error messages.";
        return false;
    }

    sensorId= "RGBDSensorClient for " + local_depthFrame_StreamingPort_name;

    if(!initialize_YARP(config) )
    {
        yCError(RGBDSENSORCLIENT) << sensorId << "\n\t* Error initializing YARP ports *";
        return false;
    }

    return true;
}


bool RGBDSensorClient::fromConfig(yarp::os::Searchable &config)
{
    // Parse LOCAL port names
    // TBD: check if user types '...' as port name, how to create RPC port names
    if (!config.check("localImagePort", "full name of the port for streaming color image"))
    {
        yCError(RGBDSENSORCLIENT) << "Missing 'localImagePort' parameter. Check you configuration file; it must be like:";
        yCError(RGBDSENSORCLIENT) << "   localImagePort:         Full name of the local port to open, e.g. /myApp/image_camera";
        return false;
    }

    local_colorFrame_StreamingPort_name  = config.find("localImagePort").asString();

    if (!config.check("localDepthPort", "full name of the port for streaming depth image"))
    {
        yCError(RGBDSENSORCLIENT) << "Missing 'localDepthPort' parameter. Check you configuration file; it must be like:";
        yCError(RGBDSENSORCLIENT) << "   localDepthPort:         Full name of the local port to open, e.g. /myApp/depth_camera";
        return false;
    }

    local_depthFrame_StreamingPort_name  = config.find("localDepthPort").asString();

    // Parse REMOTE port names
    if (!config.check("remoteImagePort", "full name of the port for streaming color image"))
    {
        yCError(RGBDSENSORCLIENT) << "Missing 'remoteImagePort' parameter. Check you configuration file; it must be like:";
        yCError(RGBDSENSORCLIENT) << "   remoteImagePort:         Full name of the port to read color images from, e.g. /robotName/image_camera";
        return false;
    }

    remote_colorFrame_StreamingPort_name  = config.find("remoteImagePort").asString();

    if (!config.check("remoteDepthPort", "full name of the port for streaming depth image"))
    {
        yCError(RGBDSENSORCLIENT) << "Missing 'remoteDepthPort' parameter. Check you configuration file; it must be like:";
        yCError(RGBDSENSORCLIENT) << "   remoteDepthPort:         Full name of the port to read depth images from, e.g. /robotName/depth_camera ";
        return false;
    }

    remote_depthFrame_StreamingPort_name  = config.find("remoteDepthPort").asString();

    // Single RPC port
    if (!config.check("localRpcPort", "full name of the port for streaming depth image"))
    {
        yCError(RGBDSENSORCLIENT) << "Missing 'localRpcPort' parameter. Check you configuration file; it must be like:";
        yCError(RGBDSENSORCLIENT) << "   localRpcPort:            Full name of the local RPC port to open, e.g. /myApp/RGBD/rpc";
        return false;
    }

    local_rpcPort_name  = config.find("localRpcPort").asString();

    if (!config.check("remoteRpcPort", "full name of the port for streaming depth image"))
    {
        yCError(RGBDSENSORCLIENT) << "Missing 'remoteRpcPort' parameter. Check you configuration file; it must be like:";
        yCError(RGBDSENSORCLIENT) << "   remoteRpcPort:         Full name of the remote RPC port, e.g. /robotName/RGBD/rpc";
        return false;
    }

    remote_rpcPort_name  = config.find("remoteRpcPort").asString();

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

    return true;
}

bool RGBDSensorClient::initialize_YARP(yarp::os::Searchable& /*config*/)
{
    bool ret;

    // Opening Streaming ports
    ret  = colorFrame_StreamingPort.open(local_colorFrame_StreamingPort_name);
    ret &= depthFrame_StreamingPort.open(local_depthFrame_StreamingPort_name);

    if(!ret)
    {
        yCError(RGBDSENSORCLIENT) << sensorId << " cannot open local streaming ports.";
        colorFrame_StreamingPort.close();
        depthFrame_StreamingPort.close();
    }

    if(! yarp::os::Network::connect(remote_colorFrame_StreamingPort_name, colorFrame_StreamingPort.getName(), image_carrier_type) )
    {
        yCError(RGBDSENSORCLIENT) << colorFrame_StreamingPort.getName() << " cannot connect to remote port " << remote_colorFrame_StreamingPort_name << "with carrier " << image_carrier_type;
        return false;
    }

    if(! yarp::os::Network::connect(remote_depthFrame_StreamingPort_name, depthFrame_StreamingPort.getName(), depth_carrier_type) )
    {
        yCError(RGBDSENSORCLIENT) << depthFrame_StreamingPort.getName() << " cannot connect to remote port " << remote_depthFrame_StreamingPort_name << "with carrier " << depth_carrier_type;
        return false;
    }


    // Single RPC port
    ret = rpcPort.open(local_rpcPort_name);

    if(!ret)
    {
        yCError(RGBDSENSORCLIENT) << sensorId << " cannot open local RPC port " << local_rpcPort_name;
        colorFrame_StreamingPort.close();
        depthFrame_StreamingPort.close();
        rpcPort.close();
    }

    if(! rpcPort.addOutput(remote_rpcPort_name) )
    {
        yCError(RGBDSENSORCLIENT) << sensorId << " cannot connect to port " << remote_rpcPort_name;
        colorFrame_StreamingPort.close();
        depthFrame_StreamingPort.close();
        rpcPort.close();
        return false;
    }

    // Check protocol version
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGBD_SENSOR);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_RGBD_PROTOCOL_VERSION);
    rpcPort.write(cmd, response);
    int major = response.get(3).asInt32();
    int minor = response.get(4).asInt32();

    if(major != RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR)
    {
        yCError(RGBDSENSORCLIENT) << "Major protocol number does not match, please verify client and server are updated. \
                    Expected: " << RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR << " received: " << major;
        return false;
    }


    if(minor != RGBD_INTERFACE_PROTOCOL_VERSION_MINOR)
    {
        yCWarning(RGBDSENSORCLIENT) << "Minor protocol number does not match, please verify client and server are updated.\
                      Expected: " << RGBD_INTERFACE_PROTOCOL_VERSION_MINOR << " received: " << minor;
    }

   /*
    * Multiple RPC ports
    *
    ret &= colorFrame_rpcPort.open(local_colorFrame_rpcPort_Name.c_str() );
    ret &= depthFrame_rpcPort.open(local_depthFrame_rpcPort_Name.c_str() );

    if(!ret)
        yCError(RGBDSENSORCLIENT) << "sensorId cannot open ports";

    // doing connections: How to correctly handle YARP_PORT_PREFIX for remote port names??
    if(! colorFrame_rpcPort.addOutput(remote_colorFrame_rpcPort_Name.c_str()) )  // This will handle local port names only
    {
        yCError(RGBDSENSORCLIENT) << sensorId << " cannot add output " << remote_colorFrame_rpcPort_Name;
        return false;
    }

    if(! depthFrame_rpcPort.addOutput(remote_depthFrame_rpcPort_Name.c_str()) )  // This will handle local port names only
    {
        yCError(RGBDSENSORCLIENT) << sensorId << " cannot add output " << remote_depthFrame_rpcPort_Name;
        return false;
    }
    */


    streamingReader->attach(&colorFrame_StreamingPort, &depthFrame_StreamingPort);

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
 * Implemented by DepthVisualParams_Forwarder
 */

/*
 * IDepthVisualParams interface. Look at IVisualParams.h for documentation
 *
 * Implemented by DepthVisualParams_Forwarder
 */


/*
 * IRGBDSensor specific interface methods
 */

bool RGBDSensorClient::getExtrinsicParam(yarp::sig::Matrix &extrinsic)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGBD_SENSOR);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_EXTRINSIC_PARAM);
    rpcPort.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if((response.get(0).asVocab32()) == VOCAB_FAILED)
    {
        extrinsic.zero();
        return false;
    }

    return Property::copyPortable(response.get(3), extrinsic);  // will it really work??
}


IRGBDSensor::RGBDSensor_status RGBDSensorClient::getSensorStatus()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGBD_SENSOR);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_STATUS);
    rpcPort.write(cmd, response);
    return static_cast<IRGBDSensor::RGBDSensor_status>(response.get(3).asInt32());
}


std::string RGBDSensorClient::getLastErrorMsg(yarp::os::Stamp* /*timeStamp*/)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGBD_SENSOR);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_ERROR_MSG);
    rpcPort.write(cmd, response);
    return response.get(3).asString();
}

bool RGBDSensorClient::getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp)
{
    return streamingReader->readRgb(rgbImage, timeStamp);
}

bool RGBDSensorClient::getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp)
{
    return streamingReader->readDepth(depthImage, timeStamp);
}

bool RGBDSensorClient::getImages(FlexImage &rgbImage, ImageOf<PixelFloat> &depthImage, Stamp *rgbStamp, Stamp *depthStamp)
{
    return streamingReader->read(rgbImage,
                                 depthImage,
                                 rgbStamp,
                                 depthStamp);
}

//
// IFrame Grabber Control 2 interface is implemented by FrameGrabberControls2_Forwarder
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

bool RGBDSensorClient::getRgbResolution(int &width, int &height)
{
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
