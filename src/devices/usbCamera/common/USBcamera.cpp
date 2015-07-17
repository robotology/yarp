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

using namespace yarp::os;
using namespace yarp::dev;


////////////////////////////////////////////////////

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

    return true;
}

bool USBCameraDriver::close(void)
{
    // close OS dependant device
	return false;
}

int USBCameraDriver::width () const
{
    return 0;
}

int USBCameraDriver::height () const
{
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



/*
// SET

bool USBCameraDriver::setBrightness(double value)
{
	return RES(system_resources)->setBrightness(value);
}
bool USBCameraDriver::setExposure(double value)
{
    return RES(system_resources)->setExposure(value);
}
bool USBCameraDriver::setSharpness(double value)
{
	return RES(system_resources)->setSharpness(value);
}
bool USBCameraDriver::setWhiteBalance(double blue, double red)
{
	return RES(system_resources)->setWhiteBalance(blue,red);
}
bool USBCameraDriver::setHue(double value)
{
	return RES(system_resources)->setHue(value);
}
bool USBCameraDriver::setSaturation(double value)
{
	return RES(system_resources)->setSaturation(value);
}
bool USBCameraDriver::setGamma(double value)
{
	return RES(system_resources)->setGamma(value);
}
bool USBCameraDriver::setShutter(double value)
{
	return RES(system_resources)->setShutter(value);
}
bool USBCameraDriver::setGain(double value)
{
	return RES(system_resources)->setGain(value);
}
bool USBCameraDriver::setIris(double value)
{
	return RES(system_resources)->setIris(value);
}

// GET

double USBCameraDriver::getBrightness()
{
	return RES(system_resources)->getBrightness();
}
double USBCameraDriver::getExposure()
{
	return RES(system_resources)->getExposure();
}
double USBCameraDriver::getSharpness()
{
	return RES(system_resources)->getSharpness();
}
bool USBCameraDriver::getWhiteBalance(double &blue, double &red)
{
	return RES(system_resources)->getWhiteBalance(blue,red);
}
double USBCameraDriver::getHue()
{
	return RES(system_resources)->getHue();
}
double USBCameraDriver::getSaturation()
{
	return RES(system_resources)->getSaturation();
}
double USBCameraDriver::getGamma()
{
	return RES(system_resources)->getGamma();
}
double USBCameraDriver::getShutter()
{
	return RES(system_resources)->getShutter();
}
double USBCameraDriver::getGain()
{
	return RES(system_resources)->getGain();
}
double USBCameraDriver::getIris()
{
	return RES(system_resources)->getIris();
}*/

//// RGB ///
USBCameraDriverRgb::USBCameraDriverRgb()  : USBCameraDriver()
{
    yTrace();
}

USBCameraDriverRgb::~USBCameraDriverRgb()
{
    yTrace();
}

bool USBCameraDriverRgb::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    return false;
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
}

USBCameraDriverRaw::~USBCameraDriverRaw()
{
    yTrace();
}

bool USBCameraDriverRaw::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    return false;
}

int USBCameraDriverRaw::width () const
{
    return 0;
}

int USBCameraDriverRaw::height () const
{
    return 0;
}

