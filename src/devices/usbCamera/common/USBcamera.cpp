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


// fake stuff, to be removed
bool USBCameraDriver::hasFeatureDC1394(int feature)
{
    yTrace();
    return false;

}
bool USBCameraDriver::setFeatureDC1394(int feature,double value)
{
    yTrace();
    return false;

}
double USBCameraDriver::getFeatureDC1394(int feature)
{
    yTrace();
    return false;

}
bool USBCameraDriver::hasOnOffDC1394(int feature)
{

    yTrace();
    return false;
}
bool USBCameraDriver::setActiveDC1394(int feature, bool onoff)
{
    yTrace();
    return false;

}
bool USBCameraDriver::getActiveDC1394(int feature)
{
    yTrace();
    return false;

}
bool USBCameraDriver::hasAutoDC1394(int feature)
{
    yTrace();
    return false;

}
bool USBCameraDriver::hasManualDC1394(int feature)
{
    yTrace();
    return false;

}
bool USBCameraDriver::hasOnePushDC1394(int feature)
{
    yTrace();
    return false;

}
bool USBCameraDriver::setModeDC1394(int feature, bool auto_onoff)
{

    yTrace();
    return false;
}
bool USBCameraDriver::getModeDC1394(int feature)
{
    yTrace();
    return false;

}
bool USBCameraDriver::setOnePushDC1394(int feature)
{
    yTrace();
    return false;

}
unsigned int USBCameraDriver::getVideoModeMaskDC1394()
{

    yTrace();
    return false;
}
bool USBCameraDriver::setVideoModeDC1394(int video_mode)
{

    yTrace();
    return false;
}
unsigned int USBCameraDriver::getVideoModeDC1394()
{
    yTrace();
    return false;

}
unsigned int USBCameraDriver::getFPSMaskDC1394()
{

    yTrace();
    return false;
}
unsigned int USBCameraDriver::getFPSDC1394()
{
    yTrace();
    return false;

}
bool USBCameraDriver::setFPSDC1394(int fps)
{
    yTrace();
    return false;

}
unsigned int USBCameraDriver::getISOSpeedDC1394()
{
    yTrace();
    return false;

}
bool USBCameraDriver::setISOSpeedDC1394(int speed)
{
    yTrace();
    return false;

}
unsigned int USBCameraDriver::getColorCodingMaskDC1394(unsigned int video_mode)
{

    yTrace();
    return false;
}
unsigned int USBCameraDriver::getColorCodingDC1394()
{

    yTrace();
    return false;
}
bool USBCameraDriver::setColorCodingDC1394(int coding)
{
    yTrace();
    return false;

}
bool USBCameraDriver::setWhiteBalanceDC1394(double b, double r)
{
    yTrace();
    return false;

}
bool USBCameraDriver::getWhiteBalanceDC1394(double &b, double &r)
{

    yTrace();
    return false;
}
bool USBCameraDriver::getFormat7MaxWindowDC1394(unsigned int &xdim,unsigned int &ydim,unsigned int &xstep,unsigned int &ystep,unsigned int &xoffstep,unsigned int &yoffstep)
{
    yTrace();
    return false;

}
bool USBCameraDriver::setFormat7WindowDC1394(unsigned int xdim,unsigned int ydim,int x0,int y0)
{
    yTrace();
    return false;

}
bool USBCameraDriver::getFormat7WindowDC1394(unsigned int &xdim,unsigned int &ydim,int &x0,int &y0)
{
    yTrace();
    return false;

}
bool USBCameraDriver::setOperationModeDC1394(bool b1394b)
{

    yTrace();
    return false;
}
bool USBCameraDriver::getOperationModeDC1394()
{
    yTrace();
    return false;
}
bool USBCameraDriver::setTransmissionDC1394(bool bTxON)
{
    yTrace();
    return false;

}
bool USBCameraDriver::getTransmissionDC1394()
{
    yTrace();
    return false;

}
/*
 * bool USBCameraDriver::setBayerDC1394(bool bON)
 * {
 *    return RES(system_resources)->setBayerDC1394(bON);
 * }
 * bool USBCameraDriver::getBayerDC1394()
 * {
 *    return RES(system_resources)->getBayerDC1394();
 * }
 */
bool USBCameraDriver::setBroadcastDC1394(bool onoff)
{

    yTrace();
    return false;
}
bool USBCameraDriver::setDefaultsDC1394()
{

    yTrace();
    return false;
}
bool USBCameraDriver::setResetDC1394()
{
    yTrace();
    return false;

}
bool USBCameraDriver::setPowerDC1394(bool onoff)
{
    yTrace();
    return false;

}
bool USBCameraDriver::setCaptureDC1394(bool bON)
{
    yTrace();
    return false;

}
bool USBCameraDriver::setBytesPerPacketDC1394(unsigned int bpp)
{
    yTrace();
    return false;

}
unsigned int USBCameraDriver::getBytesPerPacketDC1394()
{
    yTrace();
    return false;

}




