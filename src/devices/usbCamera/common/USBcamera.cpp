// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <USBcamera.hpp>
#include <yarp/os/LogStream.h>

#if defined(_MSC_VER)
    #include <WIN_camera.hpp>
#elif defined __unix
    #include <V4L_camera.hpp>
#endif

namespace yarp {
    namespace dev {
        class V4L_camera;
        class WIN_camera;
    }
}

using namespace yarp::os;
using namespace yarp::dev;


///////////////// generic device //////////////////////////

USBCameraDriver::USBCameraDriver()
{
    // initialize stuff
    yTrace();
}

USBCameraDriver::~USBCameraDriver()
{
    // delete subdevice, of any
    yTrace();
}

bool USBCameraDriver::open(yarp::os::Searchable& config)
{
    // open OS dependant device
    yTrace() << "input params are " << config.toString();

#if defined(_MSC_VER)
    os_device = (DeviceDriver*) new WIN_camera;
#elif defined __unix
    os_device = (DeviceDriver*) new V4L_camera;
#endif

    yarp::os::Property prop;
    prop.fromString(config.toString().c_str());

    switch(pixelType)
    {
        case VOCAB_PIXEL_MONO:
            prop.put("pixelType", VOCAB_PIXEL_MONO);
            break;

        case VOCAB_PIXEL_RGB:
        default:
            prop.put("pixelType", VOCAB_PIXEL_RGB);
            break;
    }
    if(!os_device->open(prop) )
        return false;

    os_device->view(deviceRgb);
    os_device->view(deviceRaw);
    os_device->view(deviceControls);
    os_device->view(deviceControls2);
    os_device->view(deviceTimed);
    os_device->view(deviceRgbVisualParam);

    if(deviceRaw)
    {
        _width = deviceRaw->width();
        _height = deviceRaw->height();
    }

    if(deviceRgb)
    {
        _width = deviceRgb->width();
        _height = deviceRgb->height();
    }
    return true;
}

bool USBCameraDriver::close(void)
{
    // close OS dependant device
    os_device->close();
    delete os_device;
	return true;
}

int USBCameraDriver::width () const
{
    if(deviceRaw)
        return deviceRaw->width();
    else if (deviceRgb)
        return deviceRgb->width();
    else
        return 0;
}

int USBCameraDriver::height () const
{
    if(deviceRaw)
        return deviceRaw->height();
    else if (deviceRgb)
        return deviceRgb->height();
    else
        return 0;
}


bool USBCameraDriver::getRawBuffer(unsigned char *buff)
{
    return false;
}

int USBCameraDriver::getRawBufferSize()
{
    return 0;
}

bool USBCameraDriver::getRgbBuffer(unsigned char *buff)
{
    return false;
}

yarp::os::Stamp USBCameraDriver::getLastInputStamp()
{
    if(deviceTimed)
        return deviceTimed->getLastInputStamp();

    return yarp::os::Stamp();
}


    // GET CONTROLS
double USBCameraDriver::getBrightness()
{
    yTrace();
    if(deviceControls)
        return deviceControls->getBrightness();
    return 0;
}

double USBCameraDriver::getExposure()
{
    if(deviceControls)
        return deviceControls->getExposure();
    return 0;
}

double USBCameraDriver::getGain()
{
    if(deviceControls)
        return deviceControls->getGain();
    return 0;
}

double USBCameraDriver::getGamma()
{
    if(deviceControls)
        return deviceControls->getGamma();
    return 0;
}

double USBCameraDriver::getHue()
{
    if(deviceControls)
        return deviceControls->getHue();
    return 0;
}

double USBCameraDriver::getIris()
{
    if(deviceControls)
        return deviceControls->getIris();
    return 0;
}

double USBCameraDriver::getSaturation()
{
    if(deviceControls)
        return deviceControls->getSaturation();
    return 0;
}

double USBCameraDriver::getSharpness()
{
    if(deviceControls)
        return deviceControls->getSharpness();
    return 0;
}

double USBCameraDriver::getShutter()
{
    if(deviceControls)
        return deviceControls->getShutter();
    return 0;
}

bool USBCameraDriver::getWhiteBalance(double& blue, double& red)
{
    if(deviceControls)
        return deviceControls->getWhiteBalance(blue, red);
    return false;
}

    // SET CONTROLS
bool USBCameraDriver::setBrightness(double v)
{
    yTrace();
    if(deviceControls)
        return deviceControls->setBrightness(v);
    return false;
}

bool USBCameraDriver::setExposure(double v)
{
    if(deviceControls)
        return deviceControls->setExposure(v);
    return false;
}

bool USBCameraDriver::setGain(double v)
{
    if(deviceControls)
        return deviceControls->setGain(v);
    return false;
}

bool USBCameraDriver::setGamma(double v)
{
    if(deviceControls)
        return deviceControls->setGamma(v);
    return false;
}

bool USBCameraDriver::setHue(double v)
{
    if(deviceControls)
        return deviceControls->setHue(v);
    return false;
}

bool USBCameraDriver::setIris(double v)
{
    if(deviceControls)
        return deviceControls->setIris(v);
    return false;
}

bool USBCameraDriver::setSaturation(double v)
{
    if(deviceControls)
        return deviceControls->setSaturation(v);
    return false;
}

bool USBCameraDriver::setSharpness(double v)
{
    if(deviceControls)
        return deviceControls->setSharpness(v);
    return false;
}

bool USBCameraDriver::setShutter(double v)
{
    if(deviceControls)
        return deviceControls->setShutter(v);
    return false;
}

bool USBCameraDriver::setWhiteBalance(double blue, double red)
{
    if(deviceControls)
        return deviceControls->setWhiteBalance(blue, red);
    return false;
}

int USBCameraDriver::getRgbHeight(){
    if(deviceRgbVisualParam)
        return deviceRgbVisualParam->getRgbHeight();
    return false;
}

int USBCameraDriver::getRgbWidth(){
    if(deviceRgbVisualParam)
        return deviceRgbVisualParam->getRgbWidth();
    return false;
}

bool USBCameraDriver::setRgbResolution(int width, int height){
    if(width<=0 || height<=0){
        yError()<<"usbCamera: invalid width or height";
        return false;
    }
    if(deviceRgbVisualParam){
        _width=width;
        _height=height;
        return deviceRgbVisualParam->setRgbResolution(width, height);
    }
    return false;
}

bool USBCameraDriver::getRgbFOV(double &horizontalFov, double &verticalFov){
    if(deviceRgbVisualParam)
        return deviceRgbVisualParam->getRgbFOV(horizontalFov, verticalFov);
    return false;
}

bool USBCameraDriver::setRgbFOV(double horizontalFov, double verticalFov){
    if(deviceRgbVisualParam)
        return deviceRgbVisualParam->setRgbFOV(horizontalFov, verticalFov);
    return false;
}

bool USBCameraDriver::getRgbIntrinsicParam(yarp::os::Property &intrinsic){
    if(deviceRgbVisualParam)
        return deviceRgbVisualParam->getRgbIntrinsicParam(intrinsic);
    return false;
}

bool USBCameraDriver::getRgbMirroring(bool &mirror){
    if(deviceRgbVisualParam)
        return deviceRgbVisualParam->getRgbMirroring(mirror);
    return false;
}

bool USBCameraDriver::setRgbMirroring(bool mirror){
    if(deviceRgbVisualParam)
        return deviceRgbVisualParam->setRgbMirroring(mirror);
    return false;
}



//// RGB ///
USBCameraDriverRgb::USBCameraDriverRgb()  : USBCameraDriver()
{
    yTrace();
    pixelType = VOCAB_PIXEL_RGB;
}

USBCameraDriverRgb::~USBCameraDriverRgb()
{
    yTrace();
}

bool USBCameraDriverRgb::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    if( (image.width() != _width) || (image.height() != _height) )
        image.resize(_width, _height);
    deviceRgb->getRgbBuffer(image.getRawImage());
    return true;
}

int USBCameraDriverRgb::width () const
{
    return USBCameraDriver::width();
}

int USBCameraDriverRgb::height () const
{
    return USBCameraDriver::height();
}


//// RAW ///
USBCameraDriverRaw::USBCameraDriverRaw()  : USBCameraDriver()
{
    yTrace();
    pixelType = VOCAB_PIXEL_MONO;
}

USBCameraDriverRaw::~USBCameraDriverRaw()
{
    yTrace();
}

bool USBCameraDriverRaw::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    if( (image.width() != _width) || (image.height() != _height) )
        image.resize(_width, _height);

    deviceRaw->getRawBuffer(image.getRawImage());
    return true;
}

int USBCameraDriverRaw::width () const
{
    return USBCameraDriver::width();
}

int USBCameraDriverRaw::height () const
{
    return USBCameraDriver::height();
}

/*  Implementation of IFrameGrabberControls2 interface
 *
 * Actual function will be implemented by OS specific devices
 */

bool USBCameraDriver::getCameraDescription(CameraDescriptor *camera)
{
    if(deviceControls2)
        return deviceControls2->getCameraDescription(camera);
    return false;
}

bool USBCameraDriver::hasFeature(int feature, bool *_hasFeature)
{
    if(deviceControls2)
        return deviceControls2->hasFeature(feature, _hasFeature);
    return false;
}

bool USBCameraDriver::setFeature(int feature, double value)
{
    if(deviceControls2)
        return deviceControls2->setFeature(feature, value);
    return false;
}

bool USBCameraDriver::getFeature(int feature, double *value)
{
    if(deviceControls2)
        return deviceControls2->getFeature(feature, value);
    return false;
}

bool USBCameraDriver::getFeature(int feature, double* value1, double* value2)
{
    if(deviceControls2)
        return deviceControls2->getFeature(feature, value1, value2);
    return false;
}

bool USBCameraDriver::setFeature(int feature, double value1, double value2)
{
    if(deviceControls2)
        return deviceControls2->setFeature(feature, value1, value2);
    return false;
}

bool USBCameraDriver::hasOnOff(int feature, bool *_hasOnOff)
{
    if(deviceControls2)
        return deviceControls2->hasOnOff(feature, _hasOnOff);
    return false;
}

bool USBCameraDriver::setActive(int feature, bool onoff)
{
    if(deviceControls2)
        return deviceControls2->setActive(feature, onoff);
    return false;
}

bool USBCameraDriver::getActive(int feature, bool *isActive)
{
    if(deviceControls2)
        return deviceControls2->getActive(feature, isActive);
    return false;
}

bool USBCameraDriver::hasAuto(int feature, bool *_hasAuto)
{
    if(deviceControls2)
        return deviceControls2->hasAuto(feature, _hasAuto);
    return false;
}

bool USBCameraDriver::hasManual(int feature, bool *_hasManual)
{
    if(deviceControls2)
        return deviceControls2->hasManual(feature, _hasManual);
    return false;
}

bool USBCameraDriver::hasOnePush(int feature, bool *_hasOnePush)
{
    if(deviceControls2)
        return deviceControls2->hasOnePush(feature, _hasOnePush);
    return false;
}

bool USBCameraDriver::setMode(int feature, FeatureMode mode)
{
    if(deviceControls2)
        return deviceControls2->setMode(feature, mode);
    return false;
}

bool USBCameraDriver::getMode(int feature, FeatureMode *mode)
{
    if(deviceControls2)
        return deviceControls2->getMode(feature, mode);
    return false;
}

bool USBCameraDriver::setOnePush(int feature)
{
    if(deviceControls2)
        return deviceControls2->setOnePush(feature);
    return false;
}

