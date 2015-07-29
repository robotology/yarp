// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
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
	return false;
}

int USBCameraDriver::width () const
{
    if(deviceRaw)
        deviceRaw->width();
    else if (deviceRgb)
        deviceRgb->width();
    else return 0;
}

int USBCameraDriver::height () const
{
    if(deviceRaw)
        deviceRaw->height();
    else if (deviceRgb)
        deviceRgb->height();
    else return 0;
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
    yarp::os::Stamp s;
	return s;
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
    return 0;
}

    // SET CONTROLS
bool USBCameraDriver::setBrightness(double v)
{
    yTrace();
    if(deviceControls)
        return deviceControls->setBrightness(v);
    return 0;
}

bool USBCameraDriver::setExposure(double v)
{
    if(deviceControls)
        return deviceControls->setExposure(v);
    return 0;
}

bool USBCameraDriver::setGain(double v)
{
    if(deviceControls)
        return deviceControls->setGain(v);
    return 0;
}

bool USBCameraDriver::setGamma(double v)
{
    if(deviceControls)
        return deviceControls->setGamma(v);
    return 0;
}

bool USBCameraDriver::setHue(double v)
{
    if(deviceControls)
        return deviceControls->setHue(v);
    return 0;
}

bool USBCameraDriver::setIris(double v)
{
    if(deviceControls)
        return deviceControls->setIris(v);
    return 0;
}

bool USBCameraDriver::setSaturation(double v)
{
    if(deviceControls)
        return deviceControls->setSaturation(v);
    return 0;
}

bool USBCameraDriver::setSharpness(double v)
{
    if(deviceControls)
        return deviceControls->setSharpness(v);
    return 0;
}

bool USBCameraDriver::setShutter(double v)
{
    if(deviceControls)
        return deviceControls->setShutter(v);
    return 0;
}

bool USBCameraDriver::setWhiteBalance(double blue, double red)
{
    if(deviceControls)
        return deviceControls->setWhiteBalance(blue, red);
    return 0;
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
    return 0;
}

int USBCameraDriverRgb::height () const
{
    return 0;
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
    return 0;
}

int USBCameraDriverRaw::height () const
{
    return 0;
}

