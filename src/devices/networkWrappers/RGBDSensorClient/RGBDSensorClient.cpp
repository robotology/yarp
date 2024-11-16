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
    if (!parseParams(config)) { return false; }

    bool ret = false;

    // Opening Streaming ports
    ret = colorFrame_StreamingPort.open(m_localImagePort);
    ret &= depthFrame_StreamingPort.open(m_localDepthPort);

    if (!ret)
    {
        yCError(RGBDSENSORCLIENT) << " cannot open local streaming ports: " << m_localImagePort << " or " << m_localDepthPort;
        colorFrame_StreamingPort.close();
        depthFrame_StreamingPort.close();
    }

    if (!yarp::os::Network::connect(m_remoteImagePort, colorFrame_StreamingPort.getName(), m_ImageCarrier))
    {
        yCError(RGBDSENSORCLIENT) << colorFrame_StreamingPort.getName() << " cannot connect to remote port " << m_remoteImagePort << "with carrier " << m_ImageCarrier;
        return false;
    }

    if (!yarp::os::Network::connect(m_remoteDepthPort, depthFrame_StreamingPort.getName(), m_DepthCarrier))
    {
        yCError(RGBDSENSORCLIENT) << depthFrame_StreamingPort.getName() << " cannot connect to remote port " << m_remoteDepthPort << "with carrier " << m_DepthCarrier;
        return false;
    }


    // Single RPC port
    ret = rpcPort.open(m_localRpcPort);

    if (!ret)
    {
        yCError(RGBDSENSORCLIENT) << " cannot open local RPC port " << m_localRpcPort;
        colorFrame_StreamingPort.close();
        depthFrame_StreamingPort.close();
        rpcPort.close();
    }

    if (!rpcPort.addOutput(m_remoteRpcPort))
    {
        yCError(RGBDSENSORCLIENT) << " cannot connect to port " << m_remoteRpcPort;
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

    if (major != RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR)
    {
        yCError(RGBDSENSORCLIENT) << "Major protocol number does not match, please verify client and server are updated. \
                    Expected: " << RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR << " received: " << major;
        return false;
    }


    if (minor != RGBD_INTERFACE_PROTOCOL_VERSION_MINOR)
    {
        yCWarning(RGBDSENSORCLIENT) << "Minor protocol number does not match, please verify client and server are updated.\
                      Expected: " << RGBD_INTERFACE_PROTOCOL_VERSION_MINOR << " received: " << minor;
    }

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
