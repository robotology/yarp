/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameGrabberMsgsImpl.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(ICHATBOTMSGSIMPL, "yarp.devices.chatBot_nws_yarp.ChatBotRPC_CallbackHelper")
}

using namespace yarp::dev;
using namespace yarp::sig;

FrameGrabberMsgsImpl::FrameGrabberMsgsImpl(IRgbVisualParams* _irgbvp, IFrameGrabberControls* _ictrls, IFrameGrabberControlsDC1394* _ictrls1394, IFrameGrabberImage* _iframegrabimg, IFrameGrabberImageRaw* _iframegrabimgraw )
{
    m_irgbvp = _irgbvp;
    m_ictrls = _ictrls;
    m_ictrls1394 = _ictrls1394;
    m_iframegrabimg = _iframegrabimg;
    m_iframegrabimgraw = _iframegrabimgraw;
}

IRGBVisualParamsMsgs_return_getRgbWidth FrameGrabberMsgsImpl::getRgbWidthRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbWidth response;

    // interface check
    if (!m_irgbvp)
    {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    int w = m_irgbvp->getRgbWidth();
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

IRGBVisualParamsMsgs_return_getRgbHeight FrameGrabberMsgsImpl::getRgbHeightRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbHeight response;

    // interface check
    if (!m_irgbvp) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    int h = m_irgbvp->getRgbHeight();
    if (h <= 0) {
        yCError(ICHATBOTMSGSIMPL) << "Could not perform getRgbHeight()";
        response.ret = ReturnValue::return_code::return_value_error_generic;
        return response;
    }

    response.ret = ReturnValue_ok;
    response.height = h;

    return response;
}

IRGBVisualParamsMsgs_return_getRgbSupportedCfg FrameGrabberMsgsImpl::getRgbSupportedConfigurationsRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbSupportedCfg response;

    // interface check
    if (!m_irgbvp) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    std::vector<CameraConfig> cfgs;
    auto ret = m_irgbvp->getRgbSupportedConfigurations(cfgs);

    if (ret)
    {
        response.ret = ret;
        response.configuration = cfgs;
        return response;
    }

    response.ret = ret;
    return response;
}

IRGBVisualParamsMsgs_return_getRgbResolution FrameGrabberMsgsImpl::getRgbResolutionRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbResolution response;

    // interface check
    if (!m_irgbvp) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    int w = 0;
    int h = 0;
    auto ret = m_irgbvp->getRgbResolution(w,h);

    if (ret) {
        response.ret = ret;
        response.width = w;
        response.height = h;
        return response;
    }

    response.ret = ret;
    return response;
}

ReturnValue FrameGrabberMsgsImpl::setRgbResolutionRPC(const std::int32_t width, const std::int32_t height)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_irgbvp) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_irgbvp->setRgbResolution(width, height);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setRgbResolutionRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IRGBVisualParamsMsgs_return_getRgbFOV FrameGrabberMsgsImpl::getRgbFOVRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbFOV response;

    // interface check
    if (!m_irgbvp) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    double hfov = 0;
    double vfov = 0;
    auto ret = m_irgbvp->getRgbFOV(hfov, vfov);

    if (ret) {
        response.ret = ret;
        response.horizontalFov = hfov;
        response.verticalFOV = vfov;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setRgbFOVRPC(const double horizontalFov, const double verticalFov)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_irgbvp) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_irgbvp->setRgbFOV(horizontalFov, verticalFov);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setRgbFOVRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IRGBVisualParamsMsgs_return_getRgbIntrinsicParam FrameGrabberMsgsImpl::getRgbIntrinsicParamRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbIntrinsicParam response;

    // interface check
    if (!m_irgbvp) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    yarp::os::Property intrinsic;
    auto ret = m_irgbvp->getRgbIntrinsicParam(intrinsic);

    if (ret) {
        response.ret = ret;
        response.params = intrinsic;
        return response;
    }

    response.ret = ret;
    return response;
}

IRGBVisualParamsMsgs_return_getRgbMirroring FrameGrabberMsgsImpl::getRgbMirroringRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IRGBVisualParamsMsgs_return_getRgbMirroring response;

    // interface check
    if (!m_irgbvp) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool mir=false;
    auto ret = m_irgbvp->getRgbMirroring(mir);

    if (ret) {
        response.ret = ret;
        response.mirror = mir;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setRgbMirroringRPC(const bool mirror)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_irgbvp) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_irgbvp->setRgbMirroring(mirror);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setRgbMirroringRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlMsgs_return_getCameraDescription FrameGrabberMsgsImpl::getCameraDescriptionRPC()
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

IFrameGrabberControlMsgs_return_hasFeature FrameGrabberMsgsImpl::hasFeatureRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_hasFeature response;

    // interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool hasfeat = false;
    auto ret = m_ictrls->hasFeature((yarp::dev::cameraFeature_id_t)feature, hasfeat);

    if (ret) {
        response.ret = ret;
        response.hasFeature = hasfeat;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setFeature1RPC(const std::int32_t feature, const double value)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls->setFeature((yarp::dev::cameraFeature_id_t)feature, value);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setFeature1RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlMsgs_return_getFeature1 FrameGrabberMsgsImpl::getFeature1RPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_getFeature1 response;

    // interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    double value = 0;
    auto ret = m_ictrls->getFeature((yarp::dev::cameraFeature_id_t)feature, value);

    if (ret) {
        response.ret = ret;
        response.value = value;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setFeature2RPC(const std::int32_t feature, const double value1, const double value2)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls->setFeature((yarp::dev::cameraFeature_id_t)feature, value1, value2);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setFeature2RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlMsgs_return_getFeature2 FrameGrabberMsgsImpl::getFeature2RPC(const std::int32_t feature)
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
    auto ret = m_ictrls->getFeature((yarp::dev::cameraFeature_id_t)feature, value1, value2);

    if (ret) {
        response.ret = ret;
        response.value1 = value1;
        response.value2 = value2;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlMsgs_return_hasOnOff FrameGrabberMsgsImpl::hasOnOffRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_hasOnOff response;

    //interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool hasonoff = false;
    auto ret = m_ictrls->hasOnOff((yarp::dev::cameraFeature_id_t)feature, hasonoff);

    if (ret) {
        response.ret = ret;
        response.HasOnOff = hasonoff;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setActiveRPC(const std::int32_t feature, const bool onoff)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls->setActive((yarp::dev::cameraFeature_id_t)feature, onoff);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setActiveRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlMsgs_return_getActive FrameGrabberMsgsImpl::getActiveRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_getActive response;

    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool isActive = false;
    auto ret = m_ictrls->getActive((yarp::dev::cameraFeature_id_t)feature, isActive);

    if (ret) {
        response.ret = ret;
        response.isActive = isActive;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlMsgs_return_hasAuto FrameGrabberMsgsImpl::hasAutoRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_hasAuto response;

   if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool hasauto = false;
    auto ret = m_ictrls->hasAuto((yarp::dev::cameraFeature_id_t)feature, hasauto);

    if (ret) {
        response.ret = ret;
        response.hasAuto = hasauto;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlMsgs_return_hasManual FrameGrabberMsgsImpl::hasManualRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_hasManual response;

    // interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool hasManual = false;
    auto ret = m_ictrls->hasManual((yarp::dev::cameraFeature_id_t)feature, hasManual);

    if (ret) {
        response.ret = ret;
        response.hasManual = hasManual;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlMsgs_return_hasOnePush FrameGrabberMsgsImpl::hasOnePushRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_hasOnePush response;

    // interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool hasOnePush = false;
    auto ret = m_ictrls->hasOnePush((yarp::dev::cameraFeature_id_t)feature, hasOnePush);

    if (ret) {
        response.ret = ret;
        response.hasOnePush = hasOnePush;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setModeRPC(const std::int32_t feature, const yarp::dev::FeatureMode mode)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls->setMode((yarp::dev::cameraFeature_id_t)feature, mode);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setModeRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlMsgs_return_getMode FrameGrabberMsgsImpl::getModeRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlMsgs_return_getMode response;

    // interface check
    if (!m_ictrls) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    yarp::dev::FeatureMode mode;
    auto ret = m_ictrls->getMode((yarp::dev::cameraFeature_id_t)feature, mode);

    if (ret) {
        response.ret = ret;
        response.mode = mode;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setOnePushRPC(const std::int32_t feature)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls->setOnePush((yarp::dev::cameraFeature_id_t)feature);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setOnePushRPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlDC1394Msgs_return_getVideoModeMaskDC1394 FrameGrabberMsgsImpl::getVideoModeMaskDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlDC1394Msgs_return_getVideoModeMaskDC1394 response;

    // interface check
    if (!m_ictrls1394) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    unsigned int val;
    auto ret = m_ictrls1394->getVideoModeMaskDC1394(val);

    if (ret) {
        response.ret = ret;
        response.val = val;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlDC1394Msgs_return_getVideoModeDC1394 FrameGrabberMsgsImpl::getVideoModeDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlDC1394Msgs_return_getVideoModeDC1394 response;

    // interface check
    if (!m_ictrls1394) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    unsigned int val;
    auto ret = m_ictrls1394->getVideoModeDC1394(val);

    if (ret) {
        response.ret = ret;
        response.val = val;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setVideoModeDC1394RPC(const std::int32_t videomode)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setVideoModeDC1394(videomode);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setVideoModeDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlDC1394Msgs_return_getFPSMaskDC1394 FrameGrabberMsgsImpl::getFPSMaskDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlDC1394Msgs_return_getFPSMaskDC1394 response;

    // interface check
    if (!m_ictrls1394) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    unsigned int val;
    auto ret = m_ictrls1394->getFPSMaskDC1394(val);

    if (ret) {
        response.ret = ret;
        response.val = val;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlDC1394Msgs_return_getFPSDC1394 FrameGrabberMsgsImpl::getFPSDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlDC1394Msgs_return_getFPSDC1394 response;

    // interface check
    if (!m_ictrls1394) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    unsigned int val;
    auto ret = m_ictrls1394->getFPSDC1394(val);

    if (ret) {
        response.ret = ret;
        response.fps = val;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setFPSDC1394RPC(const std::int32_t fps)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setFPSDC1394(fps);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setFPSDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlDC1394Msgs_return_getISOSpeedDC1394 FrameGrabberMsgsImpl::getISOSpeedDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlDC1394Msgs_return_getISOSpeedDC1394 response;

    // interface check
    if (!m_ictrls1394) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    unsigned int val = 0;
    auto ret = m_ictrls1394->getISOSpeedDC1394(val);

    if (ret) {
        response.ret = ret;
        response.speed = val;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setISOSpeedDC1394RPC(const std::int32_t speed)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setISOSpeedDC1394(speed);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setISOSpeedDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlDC1394Msgs_return_getColorCodingMaskDC1394 FrameGrabberMsgsImpl::getColorCodingMaskDC1394RPC(const std::int32_t videomode)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlDC1394Msgs_return_getColorCodingMaskDC1394 response;

    // interface check
    if (!m_ictrls1394) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    unsigned int val = 0;
    auto ret = m_ictrls1394->getColorCodingDC1394(val);

    if (ret) {
        response.ret = ret;
        response.val = val;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlDC1394Msgs_return_getColorCodingDC1394 FrameGrabberMsgsImpl::getColorCodingDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlDC1394Msgs_return_getColorCodingDC1394 response;

    // interface check
    if (!m_ictrls1394) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    unsigned int val = 0;
    auto ret = m_ictrls1394->getColorCodingDC1394(val);

    if (ret) {
        response.ret = ret;
        response.val = val;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setColorCodingDC1394RPC(const std::int32_t coding)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setColorCodingDC1394(coding);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setColorCodingDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlDC1394Msgs_return_getFormat7MaxWindowDC1394 FrameGrabberMsgsImpl::getFormat7MaxWindowDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlDC1394Msgs_return_getFormat7MaxWindowDC1394 response;

    // interface check
    if (!m_ictrls1394) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    unsigned int xdim = 0;
    unsigned int ydim = 0;
    unsigned int xstep = 0;
    unsigned int ystep = 0;
    unsigned int xoffstep = 0;
    unsigned int yoffstep = 0;
    auto ret = m_ictrls1394->getFormat7MaxWindowDC1394(xdim, ydim, xstep, ystep, xoffstep, yoffstep);

    if (ret) {
        response.ret = ret;
        response.xdim = xdim;
        response.ydim = ydim;
        response.xstep = xstep;
        response.ystep = ystep;
        response.xoffstep = xoffstep;
        response.yoffstep = yoffstep;
        return response;
    }

    response.ret = ret;
    return response;
}

IFrameGrabberControlDC1394Msgs_return_getFormat7WindowDC1394 FrameGrabberMsgsImpl::getFormat7WindowDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlDC1394Msgs_return_getFormat7WindowDC1394 response;

    // interface check
    if (!m_ictrls1394) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    unsigned int xdim = 0;
    unsigned int ydim = 0;
    int x = 0;
    int y = 0;
    auto ret = m_ictrls1394->getFormat7WindowDC1394(xdim,ydim, x,y);

    if (ret) {
        response.ret = ret;
        response.x0 = x;
        response.y0 = y;
        response.xdim = xdim;
        response.ydim = ydim;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setFormat7WindowDC1394RPC(const std::int32_t xdim, const std::int32_t ydim, const std::int32_t x0, const std::int32_t y0)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setFormat7WindowDC1394(xdim,ydim,x0,y0);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setFormat7WindowDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setOperationModeDC1394RPC(const bool b1394b)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setOperationModeDC1394(b1394b);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setOperationModeDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlDC1394Msgs_return_getOperationModeDC1394 FrameGrabberMsgsImpl::getOperationModeDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlDC1394Msgs_return_getOperationModeDC1394 response;

    // interface check
    if (!m_ictrls1394) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool b1394 = false;
    auto ret = m_ictrls1394->getOperationModeDC1394(b1394);

    if (ret) {
        response.ret = ret;
        response.b1394b = b1394;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setTransmissionDC1394RPC(const bool bTxON)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setTransmissionDC1394(bTxON);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setTransmissionDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlDC1394Msgs_return_getTransmissionDC1394 FrameGrabberMsgsImpl::getTransmissionDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlDC1394Msgs_return_getTransmissionDC1394 response;

    // interface check
    if (!m_ictrls1394) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    bool txon = false;
    auto ret = m_ictrls1394->getTransmissionDC1394(txon);

    if (ret) {
        response.ret = ret;
        response.bTxON = txon;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setBroadcastDC1394RPC(const bool onoff)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setBroadcastDC1394(onoff);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setBroadcastDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setDefaultsDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setDefaultsDC1394();
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setDefaultsDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setResetDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setResetDC1394();
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setResetDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setPowerDC1394RPC(const bool onoff)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setPowerDC1394(onoff);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setPowerDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setCaptureDC1394RPC(const bool bON)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setCaptureDC1394(bON);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setCaptureDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

IFrameGrabberControlDC1394Msgs_return_getBytesPerPacketDC1394 FrameGrabberMsgsImpl::getBytesPerPacketDC1394RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberControlDC1394Msgs_return_getBytesPerPacketDC1394 response;

    // interface check
    if (!m_ictrls1394) {
        response.ret = ReturnValue::return_code::return_value_error_not_ready;
        return response;
    }

    unsigned int bpp=0;
    auto ret = m_ictrls1394->getBytesPerPacketDC1394(bpp);

    if (ret) {
        response.ret = ret;
        response.bpp = bpp;
        return response;
    }

    response.ret = ret;
    return response;
}

yarp::dev::ReturnValue FrameGrabberMsgsImpl::setBytesPerPacketDC1394RPC(const std::int32_t bpp)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    // interface check
    if (!m_ictrls1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto ret = m_ictrls1394->setBytesPerPacketDC1394(bpp);
    if (!ret) {
        yCError(ICHATBOTMSGSIMPL) << "Could not do setBytesPerPacketDC1394RPC()";
        return ret;
    }

    return ReturnValue_ok;
}

//----------------

IFrameGrabberImageOf_return_getHeight FrameGrabberMsgsImpl::getHeightRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberImageOf_return_getHeight response;

    if (m_iframegrabimg)
    {
        int height = m_iframegrabimg->height();
        response.ret = ReturnValue_ok;
        response.val = height;
        return response;
    }

    if (m_iframegrabimgraw)
    {
        int height = m_iframegrabimgraw->height();
        response.ret = ReturnValue_ok;
        response.val = height;
        return response;
    }

    //no available interfaces
    response.val = 0;
    response.ret = ReturnValue::return_code::return_value_error_not_ready;
    return response;
}

IFrameGrabberImageOf_return_getWidth FrameGrabberMsgsImpl::getWidthRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberImageOf_return_getWidth response;

    if (m_iframegrabimg) {
        int width = m_iframegrabimg->width();
        response.ret = ReturnValue_ok;
        response.val = width;
        return response;
    }

    if (m_iframegrabimgraw) {
        int width = m_iframegrabimgraw->width();
        response.ret = ReturnValue_ok;
        response.val = width;
        return response;
    }

    // no available interfaces
    response.val = 0;
    response.ret = ReturnValue::return_code::return_value_error_not_ready;
    return response;
}

IFrameGrabberImageOf_return_getImage FrameGrabberMsgsImpl::getImageRPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberImageOf_return_getImage response;

    if (m_iframegrabimg) {
        FlexImage fimage;
        ImageOf<PixelRgb> image;
        auto ret = m_iframegrabimg->getImage(image);
        response.ret = ret;
        if (ret) {response.fImage.swap(image);}
        return response;
    }

    if (m_iframegrabimgraw) {
        FlexImage fimage;
        ImageOf<PixelMono> image;
        auto ret = m_iframegrabimgraw->getImage(image);
        response.ret = ret;
        if (ret) {response.fImage.swap(image);}
        return response;
    }

    // no available interfaces
    response.fImage.zero();
    response.ret = ReturnValue::return_code::return_value_error_not_ready;
    return response;
}

IFrameGrabberImageOf_return_getImageCrop FrameGrabberMsgsImpl::getImageCropRPC(const cropType_id_t type, const std::vector<yarp::dev::vertex_t>& vs)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    IFrameGrabberImageOf_return_getImageCrop response;

    std::vector<yarp::dev::vertex_t> vv;
    vv.resize(vs.size());
    for (size_t i = 0; i < vs.size(); i++) {
        vv[i] = vs[i];
    }

    if (m_iframegrabimg) {
        FlexImage fimage;
        ImageOf<PixelRgb> image;
        vv.resize(vs.size());
        auto ret = m_iframegrabimg->getImageCrop(type, vv, image);
        response.ret = ret;
        if (ret) {response.fImage.swap(image);}
        return response;
    }

    if (m_iframegrabimgraw) {
        FlexImage fimage;
        ImageOf<PixelMono> image;
        auto ret = m_iframegrabimgraw->getImageCrop(type, vv, image);
        response.ret = ret;
        if (ret) {response.fImage.swap(image);}
        return response;
    }

    // no available interfaces
    response.fImage.zero();
    response.ret = ReturnValue::return_code::return_value_error_not_ready;
    return response;
}
