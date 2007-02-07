// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/DragonflyDeviceDriver.h>

#include <yarp/os/Semaphore.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <ace/Log_Msg.h>
#include <ace/OS.h>

#include "FirewireCameras.h"

using namespace yarp::os;
using namespace yarp::dev;

/*
///
static double GetTimeAs_mSeconds(void)
{
	ACE_Time_Value timev = ACE_OS::gettimeofday ();
	return double(timev.sec()*1e3) + timev.usec() * 1e-3; 
}
*/

// Most of this is not used (here because the driver was ported).
class DragonflyResources
{
public:
	DragonflyResources (void) : _newFrameMutex(0),  _convImgMutex(1) 
	{
		// Variables initialization
		sizeX = 0;
		sizeY = 0;
		maxCams = 0;
		bufIndex = 0;
		_canPost = false;
		_acqStarted = false;
		_validContext = false;
		_pSubsampled_data = NULL;
		_raw_sizeX = 640;
		_raw_sizeY = 480;

        img=0;
	}

	~DragonflyResources () 
	{ 
        //		_uninitialize ();
		// To be sure - must protected against double calling
	}
	
	// Hardware-dependant variables
	enum { _num_buffers = 3 };
	int sizeX;
	int sizeY;
    int buffLength;
	int maxCams;
	int bufIndex;
	bool _canPost;
	bool _acqStarted;
	bool _validContext;
	int _raw_sizeX;
	int _raw_sizeY;
	unsigned char *_pSubsampled_data;
	
    FWCameras cam;

    unsigned char *img;

	Semaphore mutexArray[_num_buffers];
	Semaphore _newFrameMutex;
	Semaphore _convImgMutex;
	
	inline bool _initialize (const DragonflyOpenParameters& params);
	inline bool _uninitialize (void);

	inline bool _setBrightness (double value, bool bDefault=false);
	inline bool _setExposure (double value, bool bDefault=false);
	inline bool _setWhiteBalance (double redValue, double blueValue, bool bDefault=false);
	inline bool _setShutter (double value, bool bDefault=false);
	inline bool _setGain (double value, bool bDefault=false);

	inline void _subSampling(void);

};

/// full initialize and startup of the grabber.
inline bool DragonflyResources::_initialize (const DragonflyOpenParameters& params)
{
    if (img!=0)
        {
            ACE_OS::fprintf(stderr, "DragonflyResources: img pointer not null, was _initialize() already called?\n");
            return false;
        }

    bool ok = cam.init_cameras();
    if (!ok) {
        return false;
    }
    cam.SetSize(params._size_x, params._size_y);

    buffLength=cam.getBufferLength();
    sizeX=cam.getX();
    sizeY=cam.getY();

    img=new unsigned char [buffLength];

    cam.SetAuto(false);

	// Setup Camera Parameters, Magic Numbers :-)
	cam.SetBrightness(0);
	cam.SetExposure(300);
	if (params._whiteR>0) {
		printf("white balance %g %g\n", 
               (double)params._whiteR, (double)params._whiteB);
		cam.SetColor(params._whiteR,params._whiteB); 
	} else {
		cam.SetColor((float)50.0/63, (float)20.0/63); 
    }
	cam.SetShutter(params._shutter);	// x * 0.0625 = 20 mSec = 50 Hz
	cam.SetGain(params._gain);		// x * -0.0224 = -11.2dB
    //    cam.SetBrigthness(params._brightness);
    //    cam.SetExposure(params._exposure);

    cam.Capture(img);

    return true;
}

inline bool DragonflyResources::_uninitialize (void)
{
    delete [] img;
    img=0;
    cam.Shutdown();
    return true;
}

///
///
#if 0
inline bool DragonflyResources::_setBrightness (double value, bool bAuto)
{
    return false;
}

inline bool DragonflyResources::_setExposure (double value, bool bAuto)
{
    return false;
}

inline bool DragonflyResources::_setWhiteBalance (double redValue, int blueValue, bool bAuto)
{
    return false;
}

inline bool DragonflyResources::_setShutter (double value, bool bAuto)
{
    return false;
}

inline bool DragonflyResources::_setGain (double value, bool bAuto)
{
    return false;
}

inline void DragonflyResources::_subSampling(void)
{
}
#endif

inline DragonflyResources& RES(void *res) { return *(DragonflyResources *)res; }

///
///
DragonflyDeviceDriver::DragonflyDeviceDriver(void)
{
	system_resources = (void *) new DragonflyResources;
	ACE_ASSERT (system_resources != NULL);
}

DragonflyDeviceDriver::~DragonflyDeviceDriver()
{
	if (system_resources != NULL)
		delete (DragonflyResources *)system_resources;
	system_resources = NULL;
}

///
bool DragonflyDeviceDriver::open (const DragonflyOpenParameters &par)
{
	DragonflyResources& d = RES(system_resources);
	bool ret = d._initialize (par);

	return ret;
}

bool DragonflyDeviceDriver::close (void)
{
	DragonflyResources& d = RES(system_resources);

	bool ret = d._uninitialize ();

	return ret;
}

bool DragonflyDeviceDriver::getRawBuffer(unsigned char *buff)
{
    DragonflyResources& d = RES(system_resources);

    unsigned char *tmpBuff;

    d.cam.Capture(d.img);
  
    tmpBuff=d.img;

	memcpy(buff, tmpBuff, d.buffLength);

    return true;
}

bool DragonflyDeviceDriver::getRgbBuffer(unsigned char *buff)
{
    DragonflyResources& d = RES(system_resources);

    d.cam.Capture(d.img);

    unsigned  char *tmpBuff=d.img;

	memcpy(buff, tmpBuff, d.buffLength);

    return true;
}

bool DragonflyDeviceDriver::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& 
                                     image) {
    bool ok = false;

    DragonflyResources& d = RES(system_resources);

    d.cam.Capture(d.img);
    unsigned char *tmpBuff=d.img;

    image.resize(width(),height());
    if(image.getRawImageSize()==d.buffLength) {
        memcpy(image.getRawImage(), tmpBuff, d.buffLength);
        ok = true;
    }

    return ok;
}

int DragonflyDeviceDriver::getRawBufferSize()
{
    DragonflyResources& d = RES(system_resources);
    return d.buffLength;
}

int DragonflyDeviceDriver::width () const
{
	return RES(system_resources).sizeX;
}

int DragonflyDeviceDriver::height () const
{
	return RES(system_resources).sizeY;
}

bool DragonflyDeviceDriver::setBrightness (double value)
{
    fprintf(stderr, "DragonflyDeviceDriver::setBrightness not yet implemented\n");
    return false;
}

bool DragonflyDeviceDriver::setShutter(double value)
{
    fprintf(stderr, "DragonflyDeviceDriver::setShutter not yet implemented\n");
    return false;
}

bool DragonflyDeviceDriver::setGain(double value)
{
    fprintf(stderr, "DragonflyDeviceDriver::setGain yet implemented\n");
    return false;
}

double DragonflyDeviceDriver::getBrightness () const
{
    fprintf(stderr, "DragonflyDeviceDriver::getBrightness not yet implemented\n");
    return -1;
}

double DragonflyDeviceDriver::getShutter() const
{
    fprintf(stderr, "DragonflyDeviceDriver::getShutter not yet implemented\n");
    return -1;
}

double DragonflyDeviceDriver::getGain() const
{
    fprintf(stderr, "DragonflyDeviceDriver::getGain yet implemented\n");
    return -1;
}


bool DragonflyDeviceDriver::setWhiteBalance(double r, double g)
{
    fprintf(stderr, "DragonflyDeviceDriver::setWhiteBalance not yet implemented\n");
    return -1;
}

bool DragonflyDeviceDriver::getWhiteBalance (double &r, double &g) const
{
    r=-1.0;
    g=-1.0;
    fprintf(stderr, "DragonflyDeviceDriver::getWhiteBalance not yet implemented\n");
    return -1;
}
