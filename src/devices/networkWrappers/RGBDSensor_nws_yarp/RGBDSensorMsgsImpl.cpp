/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RGBDSensorMsgsImpl.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(ICHATBOTMSGSIMPL, "yarp.devices.chatBot_nws_yarp.ChatBotRPC_CallbackHelper")
}

using namespace yarp::dev;

RGBDSensorMsgsImpl::RGBDSensorMsgsImpl(yarp::dev::IRGBDSensor* _irgbd, yarp::dev::IFrameGrabberControls* _ictrls)
{
    m_irgbd = _irgbd;
    m_ictrls = _ictrls;
}

IRGBVisualParamsMsgs_return_getRgbWidth RGBDSensorMsgsImpl::getRgbWidthRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbWidth response;

    // interface check
    if (!m_irgbd)
    {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    int w = m_irgbd->getRgbWidth();
    if(w <= 0)
    {
        yCError(ICHATBOTMSGSIMPL) << "Could not perform getRgbWidth()";
        response.ret  = ReturnValue::return_code::return_value_error_generic;
        return response;
    }

    response.ret = ReturnValue_ok;
    response.width = w;

    return response;
}

IRGBVisualParamsMsgs_return_getRgbHeight RGBDSensorMsgsImpl::getRgbHeightRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbHeight response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    int h = m_irgbd->getRgbHeight();
    if (h <= 0) {
        yCError(ICHATBOTMSGSIMPL) << "Could not perform getRgbHeight()";
        response.ret = ReturnValue::return_code::return_value_error_generic;
        return response;
    }

    response.ret = ReturnValue_ok;
    response.height = h;

    return response;
}

IRGBVisualParamsMsgs_return_getRgbSupportedCfg RGBDSensorMsgsImpl::getRgbSupportedConfigurationsRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbSupportedCfg response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    std::vector<CameraConfig> cfgs;
    auto ret = m_irgbd->getRgbSupportedConfigurations(cfgs);

    if (ret)
    {
        response.ret = ret;
        response.configuration = cfgs;
        return response;
    }

    response.ret = ret;
    return response;
}

IRGBVisualParamsMsgs_return_getRgbResolution RGBDSensorMsgsImpl::getRgbResolutionRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbResolution response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    int w = 0;
    int h = 0;
    auto ret = m_irgbd->getRgbResolution(w, h);

    if (ret) {
        response.ret = ret;
        response.width = w;
        response.height = h;
        return response;
    }

    response.ret = ret;
    return response;
}

ReturnValue RGBDSensorMsgsImpl::setRgbResolutionRPC(const std::int32_t width, const std::int32_t height)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_irgbd) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_irgbd->setRgbResolution(width, height);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setRgbResolutionRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IRGBVisualParamsMsgs_return_getRgbFOV RGBDSensorMsgsImpl::getRgbFOVRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbFOV response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    double hfov = 0;
    double vfov = 0;
    auto ret = m_irgbd->getRgbFOV(hfov, vfov);

    if (ret) {
        response.ret = ret;
        response.horizontalFov = hfov;
        response.verticalFOV = vfov;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue RGBDSensorMsgsImpl::setRgbFOVRPC(const double horizontalFov, const double verticalFov)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_irgbd) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_irgbd->setRgbFOV(horizontalFov, verticalFov);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setRgbFOVRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IRGBVisualParamsMsgs_return_getRgbIntrinsicParam RGBDSensorMsgsImpl::getRgbIntrinsicParamRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbIntrinsicParam response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    yarp::os::Property intrinsic;
    auto ret = m_irgbd->getRgbIntrinsicParam(intrinsic);

    if (ret) {
        response.ret = ret;
        response.params = intrinsic;
        return response;
    }

    response.ret = ret;
    return response;
}

IRGBVisualParamsMsgs_return_getRgbMirroring RGBDSensorMsgsImpl::getRgbMirroringRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbMirroring response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool mir=false;
    auto ret = m_irgbd->getRgbMirroring(mir);

    if (ret) {
        response.ret = ret;
        response.mirror = mir;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue RGBDSensorMsgsImpl::setRgbMirroringRPC(const bool mirror)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_irgbd) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_irgbd->setRgbMirroring(mirror);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setRgbMirroringRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlMsgs_return_getCameraDescription RGBDSensorMsgsImpl::getCameraDescriptionRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_getCameraDescription response;

    // interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    yarp::dev::CameraDescriptor cam;
    auto ret = m_ictrls->getCameraDescription(cam);

    if (ret) {
        response.ret = ret;
        response.camera.busType = cam.busType;
        response.camera.deviceDescription = cam.deviceDescription;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlMsgs_return_hasFeature RGBDSensorMsgsImpl::hasFeatureRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_hasFeature response;

    // interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool hasfeat = false;
    auto ret = m_ictrls->hasFeature((cameraFeature_id_t)feature, hasfeat);

    if (ret) {
        response.ret = ret;
        response.hasFeature = hasfeat;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue RGBDSensorMsgsImpl::setFeature1RPC(const std::int32_t feature, const double value)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls->setFeature((cameraFeature_id_t)feature, value);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setFeature1RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlMsgs_return_getFeature1 RGBDSensorMsgsImpl::getFeature1RPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_getFeature1 response;

    // interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    double value = 0;
    auto ret = m_ictrls->getFeature((cameraFeature_id_t)feature, value);

    if (ret) {
        response.ret = ret;
        response.value = value;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue RGBDSensorMsgsImpl::setFeature2RPC(const std::int32_t feature, const double value1, const double value2)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls->setFeature((cameraFeature_id_t)feature, value1, value2);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setFeature2RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlMsgs_return_getFeature2 RGBDSensorMsgsImpl::getFeature2RPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_getFeature2 response;

    // interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    double value1 = 0;
    double value2 = 0;
    auto ret = m_ictrls->getFeature((cameraFeature_id_t)feature, value1, value2);

    if (ret) {
        response.ret = ret;
        response.value1 = value1;
        response.value2 = value2;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlMsgs_return_hasOnOff RGBDSensorMsgsImpl::hasOnOffRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_hasOnOff response;

    //interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool hasonoff = false;
    auto ret = m_ictrls->hasOnOff((cameraFeature_id_t)feature, hasonoff);

    if (ret) {
        response.ret = ret;
        response.HasOnOff = hasonoff;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue RGBDSensorMsgsImpl::setActiveRPC(const std::int32_t feature, const bool onoff)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls->setActive((cameraFeature_id_t)feature, onoff);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setActiveRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlMsgs_return_getActive RGBDSensorMsgsImpl::getActiveRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_getActive response;

    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool isActive = false;
    auto ret = m_ictrls->getActive((cameraFeature_id_t)feature, isActive);

    if (ret) {
        response.ret = ret;
        response.isActive = isActive;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlMsgs_return_hasAuto RGBDSensorMsgsImpl::hasAutoRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_hasAuto response;

   if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool hasauto = false;
    auto ret = m_ictrls->hasAuto((cameraFeature_id_t)feature, hasauto);

    if (ret) {
        response.ret = ret;
        response.hasAuto = hasauto;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlMsgs_return_hasManual RGBDSensorMsgsImpl::hasManualRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_hasManual response;

    // interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool hasManual = false;
    auto ret = m_ictrls->hasManual((cameraFeature_id_t)feature, hasManual);

    if (ret) {
        response.ret = ret;
        response.hasManual = hasManual;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlMsgs_return_hasOnePush RGBDSensorMsgsImpl::hasOnePushRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_hasOnePush response;

    // interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool hasOnePush = false;
    auto ret = m_ictrls->hasOnePush((cameraFeature_id_t)feature, hasOnePush);

    if (ret) {
        response.ret = ret;
        response.hasOnePush = hasOnePush;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue RGBDSensorMsgsImpl::setModeRPC(const std::int32_t feature, const yarp::dev::FeatureMode mode)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls->setMode((cameraFeature_id_t)feature, mode);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setModeRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlMsgs_return_getMode RGBDSensorMsgsImpl::getModeRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_getMode response;

    // interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    yarp::dev::FeatureMode mode;
    auto ret = m_ictrls->getMode((cameraFeature_id_t)feature, mode);

    if (ret) {
        response.ret = ret;
        response.mode = mode;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue RGBDSensorMsgsImpl::setOnePushRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls->setOnePush((cameraFeature_id_t)feature);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setOnePushRPC()";
        return ret;
    }

    return ReturnValue_ok;
}



IDepthVisualParamsMsgs_return_getDepthWidth RGBDSensorMsgsImpl::getDepthWidthRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IDepthVisualParamsMsgs_return_getDepthWidth response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    int w = m_irgbd->getDepthWidth();

    if (w<= 0) {
        response.ret = ReturnValue::return_code::return_value_error_method_failed;
        response.width = 0;
        return response;
    }

    response.width = w;
    response.ret = ReturnValue_ok;
    return response;
}

IDepthVisualParamsMsgs_return_getDepthHeight RGBDSensorMsgsImpl::getDepthHeightRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IDepthVisualParamsMsgs_return_getDepthHeight response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    int h = m_irgbd->getDepthWidth();

    if (h <= 0) {
        response.ret = ReturnValue::return_code::return_value_error_method_failed;
        response.height = 0;
        return response;
    }

    response.height = h;
    response.ret = ReturnValue_ok;
    return response;
}

IDepthVisualParamsMsgs_return_getDepthResolution RGBDSensorMsgsImpl::getDepthResolutionRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IDepthVisualParamsMsgs_return_getDepthResolution response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    int w = 0;
    int h = 0;
    auto ret = m_irgbd->getDepthResolution(w,h);

    if (ret) {
        response.ret = ret;
        response.width = w;
        response.height = h;
        return response;
    }

    response.width = 0;
    response.height = 0;
    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue RGBDSensorMsgsImpl::setDepthResolutionRPC(const std::int32_t width, const std::int32_t height)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_irgbd) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_irgbd->setDepthResolution(width, height);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setDepthResolutionRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IDepthVisualParamsMsgs_return_getDepthFOV RGBDSensorMsgsImpl::getDepthFOVRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IDepthVisualParamsMsgs_return_getDepthFOV response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    double hfov = 0;
    double vfov = 0;
    auto ret = m_irgbd->getDepthFOV(hfov,vfov);

    if (ret) {
        response.ret = ret;
        response.horizontalFov = hfov;
        response.verticalFOV = vfov;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue RGBDSensorMsgsImpl::setDepthFOVRPC(const double horizontalFov, const double verticalFov)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_irgbd) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_irgbd->setDepthFOV(horizontalFov, verticalFov);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setDepthFOVRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IDepthVisualParamsMsgs_return_getDepthAccuracy RGBDSensorMsgsImpl::getDepthAccuracyRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IDepthVisualParamsMsgs_return_getDepthAccuracy response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    double accuracy = 0;
    auto ret = m_irgbd->getDepthAccuracy(accuracy);

    if (ret) {
        response.ret = ret;
        response.accuracy = accuracy;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue RGBDSensorMsgsImpl::setDepthAccuracyRPC(const double accuracy)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_irgbd) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_irgbd->setDepthAccuracy(accuracy);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setDepthAccuracyRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IDepthVisualParamsMsgs_return_getDepthClipPlanes RGBDSensorMsgsImpl::getDepthClipPlanesRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IDepthVisualParamsMsgs_return_getDepthClipPlanes response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    double n = 0;
    double f = 0;
    auto ret = m_irgbd->getDepthClipPlanes(n,f);

    if (ret) {
        response.ret = ret;
        response.nearPlane = n;
        response.farPlane = f;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue RGBDSensorMsgsImpl::setDepthClipPlanesRPC(const double nearPlane, const double farPlane)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_irgbd) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_irgbd->setDepthClipPlanes(nearPlane,farPlane);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setDepthClipPlanesRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IDepthVisualParamsMsgs_return_getDepthMirroring RGBDSensorMsgsImpl::getDepthMirroringRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IDepthVisualParamsMsgs_return_getDepthMirroring response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool mirror = false;
    auto ret = m_irgbd->getDepthMirroring(mirror);

    if (ret) {
        response.ret = ret;
        response.mirror = mirror;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue RGBDSensorMsgsImpl::setDepthMirroringRPC(const bool mirror)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_irgbd) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_irgbd->setDepthMirroring(mirror);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setDepthMirroringRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IDepthVisualParamsMsgs_return_getDepthIntrinsicParam RGBDSensorMsgsImpl::getDepthIntrinsicParamRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IDepthVisualParamsMsgs_return_getDepthIntrinsicParam response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    yarp::os::Property intrinsic;
    auto ret = m_irgbd->getDepthIntrinsicParam(intrinsic);

    if (ret) {
        response.ret = ret;
        response.params = intrinsic;
        return response;
    }

    response.ret = ret;
    return response;
}

IRGBDMsgs_return_getExtrinsic RGBDSensorMsgsImpl::getExtrinsicParamRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBDMsgs_return_getExtrinsic response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    yarp::sig::Matrix data;
    auto ret = m_irgbd->getExtrinsicParam(data);

    if (ret) {
        response.ret = ret;
        response.matrix = data;
        return response;
    }

    response.ret = ret;
    return response;
}

IRGBDMsgs_return_getLastErrorMsg RGBDSensorMsgsImpl::getLastErrorMsgRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBDMsgs_return_getLastErrorMsg response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    std::string msg;
    yarp::os::Stamp stmp;
    auto ret = m_irgbd->getLastErrorMsg(msg, &stmp);

    if (ret) {
        response.ret = ret;
        response.errorMsg = msg;
        response.stamp = stmp;
        return response;
    }

    response.ret = ret;
    return response;
}

IRGBDMsgs_return_getRgbImage RGBDSensorMsgsImpl::getRgbImageRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBDMsgs_return_getRgbImage response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    yarp::sig::FlexImage img;
    yarp::os::Stamp stmp;
    auto ret = m_irgbd->getRgbImage(img, &stmp);

    if (ret) {
        response.ret = ret;
        response.img = img;
        response.stamp = stmp;
        return response;
    }

    response.ret = ret;
    return response;
}

IRGBDMsgs_return_getDepthImage RGBDSensorMsgsImpl::getDepthImageRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBDMsgs_return_getDepthImage response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    yarp::sig::ImageOf<yarp::sig::PixelFloat> img;
    yarp::os::Stamp stmp;
    auto ret = m_irgbd->getDepthImage(img, &stmp);

    if (ret) {
        response.ret = ret;
        response.img = img;
        response.stamp = stmp;
        return response;
    }

    response.ret = ret;
    return response;
}

IRGBDMsgs_return_getImages RGBDSensorMsgsImpl::getImagesRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBDMsgs_return_getImages response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    yarp::sig::FlexImage rgbimg;
    yarp::sig::ImageOf<yarp::sig::PixelFloat> depthimg;
    yarp::os::Stamp stmp;
    auto ret = m_irgbd->getImages(rgbimg, depthimg, &stmp);

    if (ret) {
        response.ret = ret;
        response.depthimg = depthimg;
        response.depthstamp = stmp;
        response.rgbimg = rgbimg;
        response.rgbstamp = stmp;
        return response;
    }

    response.ret = ret;
    return response;
}

IRGBDMsgs_return_getSensorStatus RGBDSensorMsgsImpl::getSensorStatusRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBDMsgs_return_getSensorStatus response;

    // interface check
    if (!m_irgbd) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    yarp::dev::IRGBDSensor::RGBDSensor_status status;
    auto ret = m_irgbd->getSensorStatus(status);

    if (ret) {
        response.ret = ret;
        response.status = status;
        return response;
    }

    response.ret = ret;
    return response;
}
