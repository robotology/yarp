/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


#include "USBcamera.h"
#include "USBcameraLogComponent.h"

#if defined(_MSC_VER)
#    include <WIN_camera.h>
#elif defined __unix
#    include <V4L_camera.h>
#endif

#include <yarp/os/LogStream.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>

#include <yarp/dev/DeviceDriver.h>

using namespace yarp::os;
using namespace yarp::dev;

///////////////// generic device //////////////////////////

USBCameraDriver::USBCameraDriver()
{
    // initialize stuff
    yCTrace(USBCAMERA);
}

USBCameraDriver::~USBCameraDriver()
{
    // delete subdevice, of any
    yCTrace(USBCAMERA);
}

bool USBCameraDriver::open(yarp::os::Searchable& config)
{
    // open OS dependant device
    yCTrace(USBCAMERA) << "input params are " << config.toString();

#if defined(_MSC_VER)
    os_device = (DeviceDriver*)new WIN_camera;
#elif defined __unix
    os_device = (DeviceDriver*)new V4L_camera;
#endif

    yarp::os::Property prop;
    prop.fromString(config.toString());
    if (!prop.check("pixelType")) {
        switch (pixelType) {
        case VOCAB_PIXEL_MONO:
            prop.put("pixelType", VOCAB_PIXEL_MONO);
            break;

        case VOCAB_PIXEL_RGB:
        default:
            prop.put("pixelType", VOCAB_PIXEL_RGB);
            break;
        }
    }
    if (!os_device->open(prop)) {
        delete os_device;
        return false;
    }

    os_device->view(frameGrabberImage);
    os_device->view(frameGrabberImageRaw);
    os_device->view(deviceControls);
    os_device->view(deviceTimed);
    os_device->view(deviceRgbVisualParam);

    if (frameGrabberImage != nullptr) {
        _width = frameGrabberImage->width();
        _height = frameGrabberImage->height();
    }

    if (frameGrabberImageRaw != nullptr) {
        _width = frameGrabberImageRaw->width();
        _height = frameGrabberImageRaw->height();
    }
    return true;
}

bool USBCameraDriver::close()
{
    // close OS dependant device
    os_device->close();
    delete os_device;
    return true;
}

int USBCameraDriver::width() const
{
    if (frameGrabberImage != nullptr) {
        return frameGrabberImage->width();
    }
    if (frameGrabberImageRaw != nullptr) {
        return frameGrabberImageRaw->width();
    }

    return 0;
}

int USBCameraDriver::height() const
{
    if (frameGrabberImage != nullptr) {
        return frameGrabberImage->height();
    }
    if (frameGrabberImageRaw != nullptr) {
        return frameGrabberImageRaw->height();
    }

    return 0;
}

yarp::os::Stamp USBCameraDriver::getLastInputStamp()
{
    if (deviceTimed != nullptr) {
        return deviceTimed->getLastInputStamp();
    }

    return yarp::os::Stamp();
}

int USBCameraDriver::getRgbHeight()
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->getRgbHeight();
    }
    return 0;
}

int USBCameraDriver::getRgbWidth()
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->getRgbWidth();
    }
    return 0;
}


yarp::dev::ReturnValue USBCameraDriver::getRgbSupportedConfigurations(std::vector<CameraConfig>& configurations)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->getRgbSupportedConfigurations(configurations);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::getRgbResolution(int& width, int& height)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->getRgbResolution(width, height);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::setRgbResolution(int width, int height)
{
    if (width <= 0 || height <= 0) {
        yCError(USBCAMERA) << "usbCamera: invalid width or height";
    return ReturnValue::return_code::return_value_error_method_failed;
    }
    if (deviceRgbVisualParam != nullptr) {
        _width = width;
        _height = height;
        return deviceRgbVisualParam->setRgbResolution(width, height);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::getRgbFOV(double& horizontalFov, double& verticalFov)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->getRgbFOV(horizontalFov, verticalFov);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::setRgbFOV(double horizontalFov, double verticalFov)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->setRgbFOV(horizontalFov, verticalFov);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::getRgbIntrinsicParam(yarp::os::Property& intrinsic)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->getRgbIntrinsicParam(intrinsic);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::getRgbMirroring(bool& mirror)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->getRgbMirroring(mirror);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::setRgbMirroring(bool mirror)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->setRgbMirroring(mirror);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}


//// RGB ///
USBCameraDriverRgb::USBCameraDriverRgb() :
        USBCameraDriver()
{
    yCTrace(USBCAMERA);
    pixelType = VOCAB_PIXEL_RGB;
}

USBCameraDriverRgb::~USBCameraDriverRgb()
{
    yCTrace(USBCAMERA);
}

yarp::dev::ReturnValue USBCameraDriverRgb::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    return frameGrabberImage->getImage(image);
}

yarp::dev::ReturnValue USBCameraDriverRgb::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    return frameGrabberImageRaw->getImage(image);
}

int USBCameraDriverRgb::width() const
{
    return USBCameraDriver::width();
}

int USBCameraDriverRgb::height() const
{
    return USBCameraDriver::height();
}

//// RAW ///
USBCameraDriverRaw::USBCameraDriverRaw() :
        USBCameraDriver()
{
    yCTrace(USBCAMERA);
    pixelType = VOCAB_PIXEL_MONO;
}

USBCameraDriverRaw::~USBCameraDriverRaw()
{
    yCTrace(USBCAMERA);
}

yarp::dev::ReturnValue USBCameraDriverRaw::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    return frameGrabberImageRaw->getImage(image);
}

int USBCameraDriverRaw::width() const
{
    return USBCameraDriver::width();
}

int USBCameraDriverRaw::height() const
{
    return USBCameraDriver::height();
}


/*  Implementation of IFrameGrabberControls2 interface
 *
 * Actual function will be implemented by OS specific devices
 */

yarp::dev::ReturnValue USBCameraDriver::getCameraDescription(yarp::dev::CameraDescriptor& camera)
{
    if (deviceControls != nullptr) {
        return deviceControls->getCameraDescription(camera);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::hasFeature(cameraFeature_id_t feature, bool& _hasFeature)
{
    if (deviceControls != nullptr) {
        return deviceControls->hasFeature(feature, _hasFeature);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::setFeature(cameraFeature_id_t feature, double value)
{
    if (deviceControls != nullptr) {
        return deviceControls->setFeature(feature, value);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::getFeature(cameraFeature_id_t feature, double& value)
{
    if (deviceControls != nullptr) {
        return deviceControls->getFeature(feature, value);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::getFeature(cameraFeature_id_t feature, double& value1, double& value2)
{
    if (deviceControls != nullptr) {
        return deviceControls->getFeature(feature, value1, value2);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::setFeature(cameraFeature_id_t feature, double value1, double value2)
{
    if (deviceControls != nullptr) {
        return deviceControls->setFeature(feature, value1, value2);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::hasOnOff(cameraFeature_id_t feature, bool& _hasOnOff)
{
    if (deviceControls != nullptr) {
        return deviceControls->hasOnOff(feature, _hasOnOff);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::setActive(cameraFeature_id_t feature, bool onoff)
{
    if (deviceControls != nullptr) {
        return deviceControls->setActive(feature, onoff);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::getActive(cameraFeature_id_t feature, bool& isActive)
{
    if (deviceControls != nullptr) {
        return deviceControls->getActive(feature, isActive);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::hasAuto(cameraFeature_id_t feature, bool& _hasAuto)
{
    if (deviceControls != nullptr) {
        return deviceControls->hasAuto(feature, _hasAuto);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::hasManual(cameraFeature_id_t feature, bool& _hasManual)
{
    if (deviceControls != nullptr) {
        return deviceControls->hasManual(feature, _hasManual);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::hasOnePush(cameraFeature_id_t feature, bool& _hasOnePush)
{
    if (deviceControls != nullptr) {
        return deviceControls->hasOnePush(feature, _hasOnePush);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::setMode(cameraFeature_id_t feature, FeatureMode mode)
{
    if (deviceControls != nullptr) {
        return deviceControls->setMode(feature, mode);
    }
    return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::getMode(cameraFeature_id_t feature, FeatureMode& mode)
{
    if (deviceControls != nullptr) {
        return deviceControls->getMode(feature, mode);
    }
   return ReturnValue::return_code::return_value_error_not_ready;
}

yarp::dev::ReturnValue USBCameraDriver::setOnePush(cameraFeature_id_t feature)
{
    if (deviceControls != nullptr) {
        return deviceControls->setOnePush(feature);
    }
   return ReturnValue::return_code::return_value_error_not_ready;
}
