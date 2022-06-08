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


bool USBCameraDriver::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig>& configurations)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->getRgbSupportedConfigurations(configurations);
    }
    return false;
}

bool USBCameraDriver::getRgbResolution(int& width, int& height)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->getRgbResolution(width, height);
    }
    return false;
}

bool USBCameraDriver::setRgbResolution(int width, int height)
{
    if (width <= 0 || height <= 0) {
        yCError(USBCAMERA) << "usbCamera: invalid width or height";
        return false;
    }
    if (deviceRgbVisualParam != nullptr) {
        _width = width;
        _height = height;
        return deviceRgbVisualParam->setRgbResolution(width, height);
    }
    return false;
}

bool USBCameraDriver::getRgbFOV(double& horizontalFov, double& verticalFov)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->getRgbFOV(horizontalFov, verticalFov);
    }
    return false;
}

bool USBCameraDriver::setRgbFOV(double horizontalFov, double verticalFov)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->setRgbFOV(horizontalFov, verticalFov);
    }
    return false;
}

bool USBCameraDriver::getRgbIntrinsicParam(yarp::os::Property& intrinsic)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->getRgbIntrinsicParam(intrinsic);
    }
    return false;
}

bool USBCameraDriver::getRgbMirroring(bool& mirror)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->getRgbMirroring(mirror);
    }
    return false;
}

bool USBCameraDriver::setRgbMirroring(bool mirror)
{
    if (deviceRgbVisualParam != nullptr) {
        return deviceRgbVisualParam->setRgbMirroring(mirror);
    }
    return false;
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

bool USBCameraDriverRgb::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    return frameGrabberImage->getImage(image);
}

bool USBCameraDriverRgb::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
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

bool USBCameraDriverRaw::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
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

bool USBCameraDriver::getCameraDescription(CameraDescriptor* camera)
{
    if (deviceControls != nullptr) {
        return deviceControls->getCameraDescription(camera);
    }
    return false;
}

bool USBCameraDriver::hasFeature(int feature, bool* _hasFeature)
{
    if (deviceControls != nullptr) {
        return deviceControls->hasFeature(feature, _hasFeature);
    }
    return false;
}

bool USBCameraDriver::setFeature(int feature, double value)
{
    if (deviceControls != nullptr) {
        return deviceControls->setFeature(feature, value);
    }
    return false;
}

bool USBCameraDriver::getFeature(int feature, double* value)
{
    if (deviceControls != nullptr) {
        return deviceControls->getFeature(feature, value);
    }
    return false;
}

bool USBCameraDriver::getFeature(int feature, double* value1, double* value2)
{
    if (deviceControls != nullptr) {
        return deviceControls->getFeature(feature, value1, value2);
    }
    return false;
}

bool USBCameraDriver::setFeature(int feature, double value1, double value2)
{
    if (deviceControls != nullptr) {
        return deviceControls->setFeature(feature, value1, value2);
    }
    return false;
}

bool USBCameraDriver::hasOnOff(int feature, bool* _hasOnOff)
{
    if (deviceControls != nullptr) {
        return deviceControls->hasOnOff(feature, _hasOnOff);
    }
    return false;
}

bool USBCameraDriver::setActive(int feature, bool onoff)
{
    if (deviceControls != nullptr) {
        return deviceControls->setActive(feature, onoff);
    }
    return false;
}

bool USBCameraDriver::getActive(int feature, bool* isActive)
{
    if (deviceControls != nullptr) {
        return deviceControls->getActive(feature, isActive);
    }
    return false;
}

bool USBCameraDriver::hasAuto(int feature, bool* _hasAuto)
{
    if (deviceControls != nullptr) {
        return deviceControls->hasAuto(feature, _hasAuto);
    }
    return false;
}

bool USBCameraDriver::hasManual(int feature, bool* _hasManual)
{
    if (deviceControls != nullptr) {
        return deviceControls->hasManual(feature, _hasManual);
    }
    return false;
}

bool USBCameraDriver::hasOnePush(int feature, bool* _hasOnePush)
{
    if (deviceControls != nullptr) {
        return deviceControls->hasOnePush(feature, _hasOnePush);
    }
    return false;
}

bool USBCameraDriver::setMode(int feature, FeatureMode mode)
{
    if (deviceControls != nullptr) {
        return deviceControls->setMode(feature, mode);
    }
    return false;
}

bool USBCameraDriver::getMode(int feature, FeatureMode* mode)
{
    if (deviceControls != nullptr) {
        return deviceControls->getMode(feature, mode);
    }
    return false;
}

bool USBCameraDriver::setOnePush(int feature)
{
    if (deviceControls != nullptr) {
        return deviceControls->setOnePush(feature);
    }
    return false;
}
