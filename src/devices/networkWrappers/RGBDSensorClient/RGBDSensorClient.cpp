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
        streamingReader(new RGBDSensor_StreamingMsgParser)
{
}

RGBDSensorClient::~RGBDSensorClient()
{
    close();
    delete streamingReader;
}

bool RGBDSensorClient::open(yarp::os::Searchable& config)
{
    if (!parseParams(config)) { return false; }

    bool ret = false;

    // Opening Streaming ports
    ret = m_colorFrame_StreamingPort.open(m_localImagePort);
    ret &= m_depthFrame_StreamingPort.open(m_localDepthPort);

    if (!ret)
    {
        yCError(RGBDSENSORCLIENT) << " cannot open local streaming ports: " << m_localImagePort << " or " << m_localDepthPort;
        m_colorFrame_StreamingPort.close();
        m_depthFrame_StreamingPort.close();
    }

    if (!yarp::os::Network::connect(m_remoteImagePort, m_colorFrame_StreamingPort.getName(), m_ImageCarrier))
    {
        yCError(RGBDSENSORCLIENT) << m_colorFrame_StreamingPort.getName() << " cannot connect to remote port " << m_remoteImagePort << "with carrier " << m_ImageCarrier;
        return false;
    }

    if (!yarp::os::Network::connect(m_remoteDepthPort, m_depthFrame_StreamingPort.getName(), m_DepthCarrier))
    {
        yCError(RGBDSENSORCLIENT) << m_depthFrame_StreamingPort.getName() << " cannot connect to remote port " << m_remoteDepthPort << "with carrier " << m_DepthCarrier;
        return false;
    }


    // RPC port
    ret = m_rpcPort.open(m_localRpcPort);
    if (!ret)
    {
        yCError(RGBDSENSORCLIENT) << " cannot open local RPC port " << m_localRpcPort;
        m_colorFrame_StreamingPort.close();
        m_depthFrame_StreamingPort.close();
        m_rpcPort.close();
    }

    if (!m_rpcPort.addOutput(m_remoteRpcPort))
    {
        yCError(RGBDSENSORCLIENT) << " cannot connect to port " << m_remoteRpcPort;
        m_colorFrame_StreamingPort.close();
        m_depthFrame_StreamingPort.close();
        m_rpcPort.close();
        return false;
    }

    if (!m_rgbdsensor_RPC.yarp().attachAsClient(m_rpcPort))
    {
        yCError(RGBDSENSORCLIENT, "Error! Cannot attach the port as a client");
        m_colorFrame_StreamingPort.close();
        m_depthFrame_StreamingPort.close();
        m_rpcPort.close();
        return false;
    }

    /*
    // Check protocol version
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGBD_SENSOR);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_RGBD_PROTOCOL_VERSION);
    m_rpcPort.write(cmd, response);
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
    */

    streamingReader->attach(&m_colorFrame_StreamingPort, &m_depthFrame_StreamingPort);

    return true;
}

bool RGBDSensorClient::close()
{
    m_colorFrame_StreamingPort.close();
    m_depthFrame_StreamingPort.close();
    m_rpcPort.close();
    return true;
}

/*
* IRGBDSensor specific interface methods
*/
ReturnValue RGBDSensorClient::getExtrinsicParam(yarp::sig::Matrix &extrinsic)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getExtrinsicParamRPC();
    extrinsic = r.matrix;
    return r.ret;
}


ReturnValue RGBDSensorClient::getSensorStatus(IRGBDSensor::RGBDSensor_status& status)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getSensorStatusRPC();
    status = r.status;
    return r.ret;
}


ReturnValue RGBDSensorClient::getLastErrorMsg(std::string& mesg, yarp::os::Stamp* timeStamp)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getLastErrorMsgRPC();
    mesg = r.errorMsg;
    if (timeStamp) {
        *timeStamp = r.stamp;
    }
    return r.ret;
}

ReturnValue RGBDSensorClient::getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp)
{
    //STREAMING IMPLEMENTATION, NO RPC
    std::lock_guard <std::mutex> lg(m_mutex);

    bool b = streamingReader->readRgb(rgbImage, timeStamp);
    if (!b) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return ReturnValue_ok;
}

ReturnValue RGBDSensorClient::getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp)
{
    //STREAMING IMPLEMENTATION, NO RPC
    std::lock_guard <std::mutex> lg(m_mutex);

    bool b = streamingReader->readDepth(depthImage, timeStamp);
    if (!b) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return ReturnValue_ok;
}

ReturnValue RGBDSensorClient::getImages(FlexImage &rgbImage, ImageOf<PixelFloat> &depthImage, Stamp *rgbStamp, Stamp *depthStamp)
{
    //STREAMING IMPLEMENTATION, NO RPC
    std::lock_guard <std::mutex> lg(m_mutex);

    bool b = streamingReader->read(rgbImage,
                                 depthImage,
                                 rgbStamp,
                                 depthStamp);
    if (!b) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return ReturnValue_ok;
}

/*
*  IRgbVisualParams interface. Look at IVisualParams.h for documentation
*/
int RGBDSensorClient::getRgbHeight()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getRgbHeightRPC();
    return r.height;
}

int RGBDSensorClient::getRgbWidth()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getRgbWidthRPC();
    return r.width;
}

ReturnValue RGBDSensorClient::getRgbSupportedConfigurations(std::vector<CameraConfig> &configurations)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getRgbSupportedConfigurationsRPC();
    configurations = r.configuration;
    return r.ret;
}

ReturnValue RGBDSensorClient::getRgbResolution(int &width, int &height)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getRgbResolutionRPC();
    height = r.height;
    width = r.width;
    return r.ret;
}

ReturnValue RGBDSensorClient::setRgbResolution(int width, int height)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setRgbResolutionRPC(width, height);
    return r;
}

ReturnValue RGBDSensorClient::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getRgbFOVRPC();
    horizontalFov = r.horizontalFov;
    verticalFov = r.verticalFOV;
    return r.ret;
}

ReturnValue RGBDSensorClient::setRgbFOV(double horizontalFov, double verticalFov)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setRgbFOVRPC(horizontalFov, verticalFov);
    return r;
}

ReturnValue RGBDSensorClient::getRgbIntrinsicParam(yarp::os::Property &intrinsic)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r =  m_rgbdsensor_RPC.getRgbIntrinsicParamRPC();
    intrinsic = r.params;
    return r.ret;
}

ReturnValue RGBDSensorClient::getRgbMirroring(bool& mirror)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r= m_rgbdsensor_RPC.getRgbMirroringRPC();
    mirror = r.mirror;
    return r.ret;
}

ReturnValue RGBDSensorClient::setRgbMirroring(bool mirror)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setRgbMirroringRPC(mirror);
    return r;
}

/*
* IDepthVisualParams interface. Look at IVisualParams.h for documentation
*/
int RGBDSensorClient::getDepthHeight()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthHeightRPC();
    return r.height;
}

int RGBDSensorClient::getDepthWidth()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthWidthRPC();
    return r.width;
}

ReturnValue RGBDSensorClient::getDepthResolution(int &width, int &height)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthResolutionRPC();
    height = r.height;
    width = r.width;
    return r.ret;
}

ReturnValue RGBDSensorClient::setDepthResolution(int width, int height)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setDepthResolutionRPC(width, height);
    return r;
}

ReturnValue RGBDSensorClient::getDepthFOV(double &horizontalFov, double &verticalFov)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthFOVRPC();
    horizontalFov = r.horizontalFov;
    verticalFov = r.verticalFOV;
    return r.ret;
}

ReturnValue RGBDSensorClient::setDepthFOV(double horizontalFov, double verticalFov)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setDepthFOVRPC(horizontalFov, verticalFov);
    return r;
}

ReturnValue RGBDSensorClient::getDepthAccuracy(double& accuracy)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthAccuracyRPC();
    accuracy = r.accuracy;
    return r.ret;
}

ReturnValue RGBDSensorClient::setDepthAccuracy(double accuracy)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setDepthAccuracyRPC(accuracy);
    return r;
}

ReturnValue RGBDSensorClient::getDepthClipPlanes(double &nearPlane, double &farPlane)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthClipPlanesRPC();
    nearPlane = r.nearPlane;
    farPlane = r.farPlane;
    return r.ret;
}

ReturnValue RGBDSensorClient::setDepthClipPlanes(double nearPlane, double farPlane)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setDepthClipPlanesRPC(nearPlane, farPlane);
    return r;
}

ReturnValue RGBDSensorClient::getDepthIntrinsicParam(yarp::os::Property &intrinsic)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthIntrinsicParamRPC();
    intrinsic = r.params;
    return r.ret;
}

ReturnValue RGBDSensorClient::getDepthMirroring(bool& mirror)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthMirroringRPC();
    mirror = r.mirror;
    return r.ret;
}

ReturnValue RGBDSensorClient::setDepthMirroring(bool mirror)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setDepthMirroringRPC(mirror);
    return r;
}

/*
* IFrameGrabberControls specific interface methods
*/
ReturnValue RGBDSensorClient::getCameraDescription(yarp::dev::CameraDescriptor& camera)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getCameraDescriptionRPC();
    camera.busType = r.camera.busType;
    camera.deviceDescription = r.camera.deviceDescription;
    return r.ret;
}

ReturnValue RGBDSensorClient::hasFeature(int feature, bool& hasFeature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.hasFeatureRPC(feature);
    hasFeature = r.hasFeature;
    return r.ret;
}

ReturnValue RGBDSensorClient::setFeature(int feature, double value)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setFeature1RPC(feature,value);
    return r;
}

ReturnValue RGBDSensorClient::getFeature(int feature, double& value)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getFeature1RPC(feature);
    value = r.value;
    return r.ret;
}

ReturnValue RGBDSensorClient::setFeature(int feature, double value1, double value2)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setFeature2RPC(feature,value1,value2);
    return r;
}

ReturnValue RGBDSensorClient::getFeature(int feature, double& value1, double& value2)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getFeature2RPC(feature);
    value1 = r.value1;
    value2 = r.value2;
    return r.ret;
}

ReturnValue RGBDSensorClient::hasOnOff(int feature, bool& HasOnOff)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.hasOnOffRPC(feature);
    HasOnOff = r.HasOnOff;
    return r.ret;
}

ReturnValue RGBDSensorClient::setActive(int feature, bool onoff)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setActiveRPC(feature,onoff);
    return r;
}

ReturnValue RGBDSensorClient::getActive(int feature, bool& isActive)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getActiveRPC(feature);
    isActive = r.isActive;
    return r.ret;
}

ReturnValue RGBDSensorClient::hasAuto(int feature, bool& hasAuto)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.hasAutoRPC(feature);
    hasAuto = r.hasAuto;
    return r.ret;
}

ReturnValue RGBDSensorClient::hasManual(int feature, bool& hasManual)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.hasManualRPC(feature);
    hasManual = r.hasManual;
    return r.ret;
}

ReturnValue RGBDSensorClient::hasOnePush(int feature, bool& hasOnePush)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.hasOnePushRPC(feature);
    hasOnePush = r.hasOnePush;
    return r.ret;
}

ReturnValue RGBDSensorClient::setMode(int feature, yarp::dev::FeatureMode mode)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setModeRPC(feature,mode);
    return r;
}

ReturnValue RGBDSensorClient::getMode(int feature, yarp::dev::FeatureMode& mode)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getModeRPC(feature);
    mode = r.mode;
    return r.ret;
}

ReturnValue RGBDSensorClient::setOnePush(int feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setOnePushRPC(feature);
    return r;
}
