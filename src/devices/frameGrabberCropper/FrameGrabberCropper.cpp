/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameGrabberCropper.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FRAMEGRABBERCROPPER, "yarp.device.frameGrabberCropper")
}

FrameGrabberCropper::~FrameGrabberCropper()
{
    close();
}


bool FrameGrabberCropper::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) { return false; }

    FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::vertices = {{m_x1, m_y1}, {m_x2, m_y2}};
    FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>::vertices = {{m_x1, m_y1}, {m_x2, m_y2}};

    return true;
}


bool FrameGrabberCropper::close()
{
    return true;
}

bool FrameGrabberCropper::attach(yarp::dev::PolyDriver* poly)
{
    if (!poly->isValid()) {
        yCError(FRAMEGRABBERCROPPER) << "The device is not valid";
        return false;
    }

    poly->view(FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::iFrameGrabberOf);
    poly->view(FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>::iFrameGrabberOf);
    poly->view(iFrameGrabberControls);
    poly->view(iFrameGrabberControlsDC1394);
    poly->view(iRgbVisualParams);
    poly->view(iPreciselyTimed);

    if (!FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::iFrameGrabberOf && !FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>::iFrameGrabberOf) {
        yCError(FRAMEGRABBERCROPPER) << "The device does not implement the required interfaces";
        return false;
    }

    return true;
}

bool FrameGrabberCropper::detach()
{
    FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::iFrameGrabberOf = nullptr;
    FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>::iFrameGrabberOf = nullptr;
    iFrameGrabberControls = nullptr;
    iFrameGrabberControlsDC1394 = nullptr;
    iRgbVisualParams = nullptr;
    iPreciselyTimed = nullptr;
    return true;
}

bool FrameGrabberCropper::attachAll(const yarp::dev::PolyDriverList& device2attach)
{
    if (device2attach.size() != 1) {
        yCError(FRAMEGRABBERCROPPER, "Expected only one device to be attached");
        return false;
    }

    return attach(device2attach[0]->poly);
}

bool FrameGrabberCropper::detachAll()
{
    return detach();
}

int FrameGrabberCropper::getRgbHeight()
{
    return FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::height();
}

int FrameGrabberCropper::getRgbWidth()
{
    return FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::width();
}

ReturnValue FrameGrabberCropper::setRgbResolution(int width, int height)
{
    if (!iRgbVisualParams || !m_forwardRgbVisualParams) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iRgbVisualParams->setRgbResolution(width, height);
}

ReturnValue FrameGrabberCropper::getRgbResolution(int& width, int& height)
{
    if (!iRgbVisualParams || !m_forwardRgbVisualParams) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iRgbVisualParams->getRgbResolution(width, height);
}

ReturnValue FrameGrabberCropper::getRgbFOV(double& horizontalFov, double& verticalFov)
{
    if (!iRgbVisualParams || !m_forwardRgbVisualParams) {
        horizontalFov = std::numeric_limits<double>::quiet_NaN();
        verticalFov = std::numeric_limits<double>::quiet_NaN();
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iRgbVisualParams->getRgbFOV(horizontalFov, verticalFov);
}

ReturnValue FrameGrabberCropper::setRgbFOV(double horizontalFov, double verticalFov)
{
    if (!iRgbVisualParams || !m_forwardRgbVisualParams) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iRgbVisualParams->setRgbFOV(horizontalFov, verticalFov);
}

ReturnValue FrameGrabberCropper::getRgbIntrinsicParam(yarp::os::Property& intrinsic)
{
    if (!iRgbVisualParams || !m_forwardRgbVisualParams) {
        intrinsic.clear();
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iRgbVisualParams->getRgbIntrinsicParam(intrinsic);
}

ReturnValue FrameGrabberCropper::getRgbMirroring(bool& mirror)
{
    if (!iRgbVisualParams || !m_forwardRgbVisualParams) {
        mirror = false;
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iRgbVisualParams->getRgbMirroring(mirror);

}

ReturnValue FrameGrabberCropper::setRgbMirroring(bool mirror)
{
    if (!iRgbVisualParams || !m_forwardRgbVisualParams) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iRgbVisualParams->setRgbMirroring(mirror);
}

yarp::dev::ReturnValue FrameGrabberCropper::getCameraDescription(CameraDescriptor& camera)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->getCameraDescription(camera);
}

yarp::dev::ReturnValue FrameGrabberCropper::hasFeature(cameraFeature_id_t feature, bool& hasFeature)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->hasFeature(feature, hasFeature);
}

yarp::dev::ReturnValue FrameGrabberCropper::setFeature(cameraFeature_id_t feature, double value)
{    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->setFeature(feature, value);
}

yarp::dev::ReturnValue FrameGrabberCropper::getFeature(cameraFeature_id_t feature, double& value)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->getFeature(feature, value);
}

yarp::dev::ReturnValue FrameGrabberCropper::setFeature(cameraFeature_id_t feature, double value1, double value2)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->setFeature(feature, value1, value2);
}

yarp::dev::ReturnValue FrameGrabberCropper::getFeature(cameraFeature_id_t feature, double& value1, double& value2)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->getFeature(feature, value1, value2);
}

yarp::dev::ReturnValue FrameGrabberCropper::hasOnOff(cameraFeature_id_t feature, bool& HasOnOff)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->hasOnOff(feature, HasOnOff);
}

yarp::dev::ReturnValue FrameGrabberCropper::setActive(cameraFeature_id_t feature, bool onoff)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->setActive(feature, onoff);
}

yarp::dev::ReturnValue FrameGrabberCropper::getActive(cameraFeature_id_t feature, bool& isActive)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->getActive(feature, isActive);
}

yarp::dev::ReturnValue FrameGrabberCropper::hasAuto(cameraFeature_id_t feature, bool& hasAuto)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->hasAuto(feature, hasAuto);
}

yarp::dev::ReturnValue FrameGrabberCropper::hasManual(cameraFeature_id_t feature, bool& hasManual)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->hasManual(feature, hasManual);
}

yarp::dev::ReturnValue FrameGrabberCropper::hasOnePush(cameraFeature_id_t feature, bool& hasOnePush)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->hasOnePush(feature, hasOnePush);
}

yarp::dev::ReturnValue FrameGrabberCropper::setMode(cameraFeature_id_t feature, FeatureMode mode)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->setMode(feature, mode);
}

yarp::dev::ReturnValue FrameGrabberCropper::getMode(cameraFeature_id_t feature, FeatureMode& mode)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->getMode(feature, mode);
}

yarp::dev::ReturnValue FrameGrabberCropper::setOnePush(cameraFeature_id_t feature)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControls->setOnePush(feature);
}

yarp::dev::ReturnValue FrameGrabberCropper::getVideoModeMaskDC1394(unsigned int& val)
{
    if (!iFrameGrabberControlsDC1394) {
        val = 0;
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->getVideoModeMaskDC1394(val);
}

yarp::dev::ReturnValue FrameGrabberCropper::getVideoModeDC1394(unsigned int& val)
{
    if (!iFrameGrabberControlsDC1394) {
        val = 0;
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->getVideoModeMaskDC1394(val);
}

ReturnValue FrameGrabberCropper::setVideoModeDC1394(int video_mode)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setVideoModeDC1394(video_mode);
}

yarp::dev::ReturnValue FrameGrabberCropper::getFPSMaskDC1394(unsigned int& val)
{
    if (!iFrameGrabberControlsDC1394) {
        val = 0;
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->getFPSMaskDC1394(val);
}

yarp::dev::ReturnValue FrameGrabberCropper::getFPSDC1394(unsigned int& val)
{
    if (!iFrameGrabberControlsDC1394) {
        val = 0;
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->getFPSDC1394(val);
}

ReturnValue FrameGrabberCropper::setFPSDC1394(int fps)

{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setFPSDC1394(fps);
}

yarp::dev::ReturnValue FrameGrabberCropper::getISOSpeedDC1394(unsigned int& val)
{
    if (!iFrameGrabberControlsDC1394) {
        val = 0;
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->getISOSpeedDC1394(val);
}

ReturnValue FrameGrabberCropper::setISOSpeedDC1394(int speed)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setISOSpeedDC1394(speed);
}

yarp::dev::ReturnValue FrameGrabberCropper::getColorCodingMaskDC1394(unsigned int video_mode,unsigned int& val)
{
    if (!iFrameGrabberControlsDC1394) {
        val = 0;
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->getColorCodingMaskDC1394(video_mode,val);
}

yarp::dev::ReturnValue FrameGrabberCropper::getColorCodingDC1394(unsigned int& val)
{
    if (!iFrameGrabberControlsDC1394) {
        val = 0;
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->getColorCodingDC1394(val);
}

ReturnValue FrameGrabberCropper::setColorCodingDC1394(int coding)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setColorCodingDC1394(coding);
}

ReturnValue FrameGrabberCropper::getFormat7MaxWindowDC1394(unsigned int& xdim,
                                unsigned int& ydim,
                                unsigned int& xstep,
                                unsigned int& ystep,
                                unsigned int& xoffstep,
                                unsigned int& yoffstep)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->getFormat7MaxWindowDC1394(xdim, ydim, xstep, ystep, xoffstep, yoffstep);
}

ReturnValue FrameGrabberCropper::getFormat7WindowDC1394(unsigned int& xdim, unsigned int& ydim, int& x0, int& y0)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->getFormat7WindowDC1394(xdim, ydim, x0, y0);
}

ReturnValue FrameGrabberCropper::setFormat7WindowDC1394(unsigned int xdim, unsigned int ydim, int x0, int y0)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setFormat7WindowDC1394(xdim, ydim, x0, y0);
}

ReturnValue FrameGrabberCropper::setOperationModeDC1394(bool b1394b)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setOperationModeDC1394(b1394b);
}

ReturnValue FrameGrabberCropper::getOperationModeDC1394(bool& b1394b)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->getOperationModeDC1394(b1394b);
}

ReturnValue FrameGrabberCropper::setTransmissionDC1394(bool bTxON)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setTransmissionDC1394(bTxON);
}

ReturnValue FrameGrabberCropper::getTransmissionDC1394(bool& bTxON)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->getTransmissionDC1394(bTxON);
}

ReturnValue FrameGrabberCropper::setBroadcastDC1394(bool onoff)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setBroadcastDC1394(onoff);
}

ReturnValue FrameGrabberCropper::setDefaultsDC1394()
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setDefaultsDC1394();
}

ReturnValue FrameGrabberCropper::setResetDC1394()
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setResetDC1394();
}

ReturnValue FrameGrabberCropper::setPowerDC1394(bool onoff)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setPowerDC1394(onoff);
}

ReturnValue FrameGrabberCropper::setCaptureDC1394(bool bON)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setCaptureDC1394(bON);
}

ReturnValue FrameGrabberCropper::getBytesPerPacketDC1394(unsigned int& bpp)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->getBytesPerPacketDC1394(bpp);
}

ReturnValue FrameGrabberCropper::setBytesPerPacketDC1394(unsigned int bpp)
{
    if (!iFrameGrabberControlsDC1394) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iFrameGrabberControlsDC1394->setBytesPerPacketDC1394(bpp);
}

ReturnValue FrameGrabberCropper::getRgbSupportedConfigurations(std::vector<yarp::dev::CameraConfig>& cfgs)
{
    if (!iFrameGrabberControls) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }
    return iRgbVisualParams->getRgbSupportedConfigurations(cfgs);
}
