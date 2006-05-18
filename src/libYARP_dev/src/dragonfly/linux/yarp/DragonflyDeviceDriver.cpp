// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/DragonflyDeviceDriver.h>

#include <yarp/os/Semaphore.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/FrameGrabber.h>
#include <ace/Log_Msg.h>

#include "FirewireCameras.h"

using namespace yarp::os;
using namespace yarp::dev;

///
double GetTimeAs_mSeconds(void)
{
	ACE_Time_Value timev = ACE_OS::gettimeofday ();
	return double(timev.sec()*1e3) + timev.usec() * 1e-3; 
}

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

	inline bool _setBrightness (int value, bool bDefault=false);
	inline bool _setExposure (int value, bool bDefault=false);
	inline bool _setWhiteBalance (int redValue, int blueValue, bool bDefault=false);
	inline bool _setShutter (int value, bool bDefault=false);
	inline bool _setGain (int value, bool bDefault=false);

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

    cam.init_cameras();

    buffLength=cam.getBufferLength();
    sizeX=cam.getX();
    sizeY=cam.getY();

    img=new unsigned char [buffLength];

    cam.SetAuto(true);
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
inline bool DragonflyResources::_setBrightness (int value, bool bAuto)
{
    return false;
}

inline bool DragonflyResources::_setExposure (int value, bool bAuto)
{
    return false;
}

inline bool DragonflyResources::_setWhiteBalance (int redValue, int blueValue, bool bAuto)
{
    return false;
}

inline bool DragonflyResources::_setShutter (int value, bool bAuto)
{
    return false;
}

inline bool DragonflyResources::_setGain (int value, bool bAuto)
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
    //	YARPScheduler::setHighResScheduling ();

	return ret;
}

bool DragonflyDeviceDriver::close (void)
{
	DragonflyResources& d = RES(system_resources);

	bool ret = d._uninitialize ();

	return ret;
}

bool DragonflyDeviceDriver::acquireBuffer (void *buffer)
{
    DragonflyResources& d = RES(system_resources);
  
    (*(unsigned char **)buffer) = d.img;
  
    return true;
}

bool DragonflyDeviceDriver::releaseBuffer ()
{	
    return true;
}

bool DragonflyDeviceDriver::waitOnNewFrame ()
{
    DragonflyResources& d = RES(system_resources);
  
    d.cam.Capture(d.img);
	
    return true;
}

bool DragonflyDeviceDriver::getBuffer(unsigned char *buff)
{
    DragonflyResources& d = RES(system_resources);

    char *tmpBuff;
    waitOnNewFrame ();
	acquireBuffer(&tmpBuff);

	memcpy(buff, tmpBuff, d.buffLength);

	releaseBuffer ();
}

int DragonflyDeviceDriver::getWidth ()
{
	return RES(system_resources).sizeX;
}

int DragonflyDeviceDriver::getHeight ()
{
	return RES(system_resources).sizeY;
}

#if 0
int YARPDragonflyDeviceDriver::setBrightness (void *cmd)
{
    /// RES(system_resources)._setBrightness(*cmd);
    return YARP_OK;
}

int YARPDragonflyDeviceDriver::setHue (void *cmd)
{
    /// RES(system_resources)._setHue(*cmd);
    return YARP_OK;
}

int YARPDragonflyDeviceDriver::setContrast (void *cmd)
{
    /// RES(system_resources)._setContrast(*cmd);
    return YARP_OK;
}

int YARPDragonflyDeviceDriver::setSatU (void *cmd)
{
    /// RES(system_resources)._setSatU(*cmd);
    return YARP_OK;
}

int YARPDragonflyDeviceDriver::setSatV (void *cmd)
{
    /// RES(system_resources)._setSatV(*cmd);
    return YARP_OK;
}

int YARPDragonflyDeviceDriver::setLNotch (void *cmd)
{
    /// RES(system_resources)._setLNotch(*cmd);
    return YARP_OK;
}

int YARPDragonflyDeviceDriver::setLDec (void *cmd)
{
    /// RES(system_resources)._setLDec(*cmd);
    return YARP_OK;
}

int YARPDragonflyDeviceDriver::setCrush (void *cmd)
{
    /// RES(system_resources)._setCrush(*cmd);
    return YARP_OK;
}

#endif
