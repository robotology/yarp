/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RGBDSensor_nwc_yarp.h"
#include "RGBDSensor_nwc_yarp_StreamingMsgParser.h"
#include <yarp/os/Portable.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

YARP_LOG_COMPONENT(RGBDSENSOR_NWC_YARP, "yarp.devices.RGBDSensor_nwc_yarp")


RGBDSensor_nwc_yarp::RGBDSensor_nwc_yarp() :
        streamingReader(new RGBDSensor_StreamingMsgParser)
{
}

RGBDSensor_nwc_yarp::~RGBDSensor_nwc_yarp()
{
    close();
    delete streamingReader;
}

bool RGBDSensor_nwc_yarp::open(yarp::os::Searchable& config)
{
    if (!parseParams(config)) { return false; }

    bool ret = false;

    // Opening Streaming ports
    ret = m_colorFrame_StreamingPort.open(m_localImagePort);
    ret &= m_depthFrame_StreamingPort.open(m_localDepthPort);

    if (!ret)
    {
        yCError(RGBDSENSOR_NWC_YARP) << " cannot open local streaming ports: " << m_localImagePort << " or " << m_localDepthPort;
        m_colorFrame_StreamingPort.close();
        m_depthFrame_StreamingPort.close();
    }

    if (!yarp::os::Network::connect(m_remoteImagePort, m_colorFrame_StreamingPort.getName(), m_ImageCarrier))
    {
        yCError(RGBDSENSOR_NWC_YARP) << m_colorFrame_StreamingPort.getName() << " cannot connect to remote port " << m_remoteImagePort << "with carrier " << m_ImageCarrier;
        return false;
    }

    if (!yarp::os::Network::connect(m_remoteDepthPort, m_depthFrame_StreamingPort.getName(), m_DepthCarrier))
    {
        yCError(RGBDSENSOR_NWC_YARP) << m_depthFrame_StreamingPort.getName() << " cannot connect to remote port " << m_remoteDepthPort << "with carrier " << m_DepthCarrier;
        return false;
    }


    // RPC port
    ret = m_rpcPort.open(m_localRpcPort);
    if (!ret)
    {
        yCError(RGBDSENSOR_NWC_YARP) << " cannot open local RPC port " << m_localRpcPort;
        m_colorFrame_StreamingPort.close();
        m_depthFrame_StreamingPort.close();
        m_rpcPort.close();
    }

    if (!m_rpcPort.addOutput(m_remoteRpcPort))
    {
        yCError(RGBDSENSOR_NWC_YARP) << " cannot connect to port " << m_remoteRpcPort;
        m_colorFrame_StreamingPort.close();
        m_depthFrame_StreamingPort.close();
        m_rpcPort.close();
        return false;
    }

    if (!m_rgbdsensor_RPC.yarp().attachAsClient(m_rpcPort))
    {
        yCError(RGBDSENSOR_NWC_YARP, "Error! Cannot attach the port as a client");
        m_colorFrame_StreamingPort.close();
        m_depthFrame_StreamingPort.close();
        m_rpcPort.close();
        return false;
    }

    // Check the protocol version
    if (!m_rgbdsensor_RPC.checkProtocolVersion()) {
        return false;
    }

    streamingReader->attach(&m_colorFrame_StreamingPort, &m_depthFrame_StreamingPort);

    return true;
}

bool RGBDSensor_nwc_yarp::close()
{
    m_colorFrame_StreamingPort.close();
    m_depthFrame_StreamingPort.close();
    m_rpcPort.close();
    return true;
}

/*
* IRGBDSensor specific interface methods
*/
ReturnValue RGBDSensor_nwc_yarp::getExtrinsicParam(yarp::sig::Matrix &extrinsic)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getExtrinsicParamRPC();
    extrinsic = r.matrix;
    return r.ret;
}


ReturnValue RGBDSensor_nwc_yarp::getSensorStatus(IRGBDSensor::RGBDSensor_status& status)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getSensorStatusRPC();
    status = r.status;
    return r.ret;
}


ReturnValue RGBDSensor_nwc_yarp::getLastErrorMsg(std::string& mesg, yarp::os::Stamp* timeStamp)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getLastErrorMsgRPC();
    mesg = r.errorMsg;
    if (timeStamp) {
        *timeStamp = r.stamp;
    }
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp)
{
    //STREAMING IMPLEMENTATION, NO RPC
    std::lock_guard <std::mutex> lg(m_mutex);

    bool b = streamingReader->readRgb(rgbImage, timeStamp);
    if (!b) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return ReturnValue_ok;
}

ReturnValue RGBDSensor_nwc_yarp::getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp)
{
    //STREAMING IMPLEMENTATION, NO RPC
    std::lock_guard <std::mutex> lg(m_mutex);

    bool b = streamingReader->readDepth(depthImage, timeStamp);
    if (!b) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return ReturnValue_ok;
}

ReturnValue RGBDSensor_nwc_yarp::getImages(FlexImage &rgbImage, ImageOf<PixelFloat> &depthImage, Stamp *rgbStamp, Stamp *depthStamp)
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
int RGBDSensor_nwc_yarp::getRgbHeight()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getRgbHeightRPC();
    return r.height;
}

int RGBDSensor_nwc_yarp::getRgbWidth()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getRgbWidthRPC();
    return r.width;
}

ReturnValue RGBDSensor_nwc_yarp::getRgbSupportedConfigurations(std::vector<CameraConfig> &configurations)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getRgbSupportedConfigurationsRPC();
    configurations = r.configuration;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::getRgbResolution(int &width, int &height)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getRgbResolutionRPC();
    height = r.height;
    width = r.width;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setRgbResolution(int width, int height)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setRgbResolutionRPC(width, height);
    return r;
}

ReturnValue RGBDSensor_nwc_yarp::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getRgbFOVRPC();
    horizontalFov = r.horizontalFov;
    verticalFov = r.verticalFOV;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setRgbFOV(double horizontalFov, double verticalFov)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setRgbFOVRPC(horizontalFov, verticalFov);
    return r;
}

ReturnValue RGBDSensor_nwc_yarp::getRgbIntrinsicParam(yarp::os::Property &intrinsic)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r =  m_rgbdsensor_RPC.getRgbIntrinsicParamRPC();
    intrinsic = r.params;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::getRgbMirroring(bool& mirror)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r= m_rgbdsensor_RPC.getRgbMirroringRPC();
    mirror = r.mirror;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setRgbMirroring(bool mirror)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setRgbMirroringRPC(mirror);
    return r;
}

/*
* IDepthVisualParams interface. Look at IVisualParams.h for documentation
*/
int RGBDSensor_nwc_yarp::getDepthHeight()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthHeightRPC();
    return r.height;
}

int RGBDSensor_nwc_yarp::getDepthWidth()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthWidthRPC();
    return r.width;
}

ReturnValue RGBDSensor_nwc_yarp::getDepthResolution(int &width, int &height)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthResolutionRPC();
    height = r.height;
    width = r.width;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setDepthResolution(int width, int height)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setDepthResolutionRPC(width, height);
    return r;
}

ReturnValue RGBDSensor_nwc_yarp::getDepthFOV(double &horizontalFov, double &verticalFov)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthFOVRPC();
    horizontalFov = r.horizontalFov;
    verticalFov = r.verticalFOV;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setDepthFOV(double horizontalFov, double verticalFov)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setDepthFOVRPC(horizontalFov, verticalFov);
    return r;
}

ReturnValue RGBDSensor_nwc_yarp::getDepthAccuracy(double& accuracy)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthAccuracyRPC();
    accuracy = r.accuracy;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setDepthAccuracy(double accuracy)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setDepthAccuracyRPC(accuracy);
    return r;
}

ReturnValue RGBDSensor_nwc_yarp::getDepthClipPlanes(double &nearPlane, double &farPlane)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthClipPlanesRPC();
    nearPlane = r.nearPlane;
    farPlane = r.farPlane;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setDepthClipPlanes(double nearPlane, double farPlane)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setDepthClipPlanesRPC(nearPlane, farPlane);
    return r;
}

ReturnValue RGBDSensor_nwc_yarp::getDepthIntrinsicParam(yarp::os::Property &intrinsic)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthIntrinsicParamRPC();
    intrinsic = r.params;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::getDepthMirroring(bool& mirror)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getDepthMirroringRPC();
    mirror = r.mirror;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setDepthMirroring(bool mirror)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setDepthMirroringRPC(mirror);
    return r;
}

/*
* IFrameGrabberControls specific interface methods
*/
ReturnValue RGBDSensor_nwc_yarp::getCameraDescription(yarp::dev::CameraDescriptor& camera)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getCameraDescriptionRPC();
    camera.busType = r.camera.busType;
    camera.deviceDescription = r.camera.deviceDescription;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::hasFeature(cameraFeature_id_t feature, bool& hasFeature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.hasFeatureRPC((int32_t)feature);
    hasFeature = r.hasFeature;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setFeature(cameraFeature_id_t feature, double value)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setFeature1RPC((int32_t)feature, value);
    return r;
}

ReturnValue RGBDSensor_nwc_yarp::getFeature(cameraFeature_id_t feature, double& value)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getFeature1RPC((int32_t)feature);
    value = r.value;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setFeature(cameraFeature_id_t feature, double value1, double value2)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setFeature2RPC((int32_t)feature, value1, value2);
    return r;
}

ReturnValue RGBDSensor_nwc_yarp::getFeature(cameraFeature_id_t feature, double& value1, double& value2)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getFeature2RPC((int32_t)feature);
    value1 = r.value1;
    value2 = r.value2;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::hasOnOff(cameraFeature_id_t feature, bool& HasOnOff)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.hasOnOffRPC((int32_t)feature);
    HasOnOff = r.HasOnOff;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setActive(cameraFeature_id_t feature, bool onoff)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setActiveRPC((int32_t)feature, onoff);
    return r;
}

ReturnValue RGBDSensor_nwc_yarp::getActive(cameraFeature_id_t feature, bool& isActive)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getActiveRPC((int32_t)feature);
    isActive = r.isActive;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::hasAuto(cameraFeature_id_t feature, bool& hasAuto)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.hasAutoRPC((int32_t)feature);
    hasAuto = r.hasAuto;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::hasManual(cameraFeature_id_t feature, bool& hasManual)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.hasManualRPC((int32_t)feature);
    hasManual = r.hasManual;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::hasOnePush(cameraFeature_id_t feature, bool& hasOnePush)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.hasOnePushRPC((int32_t)feature);
    hasOnePush = r.hasOnePush;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setMode(cameraFeature_id_t feature, yarp::dev::FeatureMode mode)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setModeRPC((int32_t)feature, mode);
    return r;
}

ReturnValue RGBDSensor_nwc_yarp::getMode(cameraFeature_id_t feature, yarp::dev::FeatureMode& mode)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.getModeRPC((int32_t)feature);
    mode = r.mode;
    return r.ret;
}

ReturnValue RGBDSensor_nwc_yarp::setOnePush(cameraFeature_id_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto r = m_rgbdsensor_RPC.setOnePushRPC((int32_t)feature);
    return r;
}
