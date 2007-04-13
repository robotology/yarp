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

#include "FirewireCameraSet.h"

using namespace yarp::os;
using namespace yarp::dev;

// Most of this is not used (here because the driver was ported).
class DragonflyResources
{
public:
	DragonflyResources (void) //: _newFrameMutex(0),  _convImgMutex(1) 
	{
	    /*
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
        */
        
        //img=0;
	}

	~DragonflyResources () 
	{ 
        _uninitialize();
	}
	
	
	// Hardware-dependant variables
	enum { _num_buffers = 3 };
	/*	
	int maxCams;
	int bufIndex;
	unsigned int unit_number;
	bool _canPost;
	bool _acqStarted;
	bool _validContext;
	int _raw_sizeX;
	int _raw_sizeY;
	unsigned char *_pSubsampled_data;
	*/
	
    static CFWCameraSet* m_pCameraSet;
    static Semaphore m_InitCloseMutex;

    //unsigned char *img;
    
    unsigned int unit_number;

	int sizeX;
	int sizeY;
    int buffLength;

	//Semaphore mutexArray[_num_buffers];
	//Semaphore _newFrameMutex;
	//Semaphore _convImgMutex;
	
	inline bool _initialize (const DragonflyOpenParameters& params);
	inline bool _uninitialize();

	inline bool _setBrightness (double value);
	inline bool _setWhiteBalance (double blueValue, double redValue);
	inline bool _setShutter (double value);
	inline bool _setGain (double value);

    inline double _getBrightness();
    inline bool _getWhiteBalance(double& blue,double& red);
    inline double _getShutter();
    inline double _getGain();

    inline bool _setAuto(bool bAuto);

	inline bool _setAutoBrightness(bool bAuto=true);
	inline bool _setAutoWhiteBalance(bool bAuto=true);
	inline bool _setAutoShutter(bool bAuto=true);
	inline bool _setAutoGain(bool bAuto=true);

    inline void _printSettings()
    {
        m_pCameraSet->PrintSettings(unit_number);
    }

	//inline void _subSampling();
	
    inline bool _capture(unsigned char *buff)
    {
        return m_pCameraSet->Capture(unit_number,buff);
    }

    inline bool _capture_raw(unsigned char *buff)
    {
        return m_pCameraSet->CaptureRaw(unit_number,buff);
    }
};

CFWCameraSet* DragonflyResources::m_pCameraSet=NULL;
Semaphore DragonflyResources::m_InitCloseMutex;

/// full initialize and startup of the grabber.
inline bool DragonflyResources::_initialize (const DragonflyOpenParameters& params)
{
    m_InitCloseMutex.wait();
    
    if (!m_pCameraSet)
    {
        m_pCameraSet=new CFWCameraSet();
        
        if (!m_pCameraSet->Init()) // default port=0
        {
            delete m_pCameraSet;
            m_pCameraSet=0;
            m_InitCloseMutex.post(); 
            return false;
        }
    }

    unit_number=params._unit_number;
    sizeX=params._size_x;
    sizeY=params._size_y;
    buffLength=sizeX*sizeY*3;

    if (!m_pCameraSet->StartCamera(unit_number,sizeX,sizeY)) // default dma=true
    {
        ACE_OS::fprintf(stderr, "DragonflyResources: can't open camera %d",unit_number);
        return false;
    }
    
    m_InitCloseMutex.post();
    
    //img=new unsigned char [buffLength];

    //cam.SetAuto(false);

	// Setup Camera Parameters, Magic Numbers :-)
	m_pCameraSet->SetBrightness(unit_number,params._brightness);	
	m_pCameraSet->SetShutter(unit_number,params._shutter);
	m_pCameraSet->SetGain(unit_number,params._gain);
	
	if (params._whiteR>0.0) 
	{
		m_pCameraSet->SetWhiteBalance(unit_number,params._whiteB,params._whiteR); 
	} 
	else 
	{
		m_pCameraSet->SetWhiteBalance(unit_number,0.5,0.5); 
    }

    return true;
}

inline bool DragonflyResources::_uninitialize()
{
    m_InitCloseMutex.wait();
    
    /*
    if (img)
    {
        delete [] img;
        img=0;
    }
    */

    if (m_pCameraSet)
    {
        m_pCameraSet->ShutdownCamera(unit_number);
    
        if (m_pCameraSet->GetCameraNum()<=0)
        {
            m_pCameraSet->Shutdown();
            delete m_pCameraSet;
            m_pCameraSet=0;
        }
    }
    
    m_InitCloseMutex.post();
    
    return true;
}

inline bool DragonflyResources::_setBrightness(double value)
{
    return m_pCameraSet->SetBrightness(unit_number,value);
}

inline bool DragonflyResources::_setWhiteBalance(double blue, double red)
{
    return m_pCameraSet->SetWhiteBalance(unit_number,blue,red);
}

inline bool DragonflyResources::_setShutter(double value)
{
    return m_pCameraSet->SetShutter(unit_number,value);
}

inline bool DragonflyResources::_setGain(double value)
{
    return m_pCameraSet->SetGain(unit_number,value);
}

inline double DragonflyResources::_getBrightness()
{
    return m_pCameraSet->GetBrightness(unit_number);
}

inline bool DragonflyResources::_getWhiteBalance(double& blue, double& red)
{
    return m_pCameraSet->GetWhiteBalance(unit_number,blue,red);
}

inline double DragonflyResources::_getShutter()
{
    return m_pCameraSet->GetShutter(unit_number);
}

inline double DragonflyResources::_getGain()
{
    return m_pCameraSet->GetGain(unit_number);
}

inline bool DragonflyResources::_setAuto(bool bAuto)
{
    bool bOk=true;

    bOk=bOk && _setAutoBrightness(bAuto);
    bOk=bOk && _setAutoWhiteBalance(bAuto);
    bOk=bOk && _setAutoShutter(bAuto);
    bOk=bOk && _setAutoGain(bAuto);

    return bOk;
}

inline bool DragonflyResources::_setAutoBrightness(bool bAuto)
{
    return m_pCameraSet->SetAutoBrightness(unit_number,bAuto);
}

inline bool DragonflyResources::_setAutoWhiteBalance(bool bAuto)
{
    return m_pCameraSet->SetAutoWhiteBalance(unit_number,bAuto);
}

inline bool DragonflyResources::_setAutoShutter(bool bAuto)
{
    return m_pCameraSet->SetAutoShutter(unit_number,bAuto);
}

inline bool DragonflyResources::_setAutoGain(bool bAuto)
{
    return m_pCameraSet->SetAutoGain(unit_number,bAuto);
}

/*
inline void DragonflyResources::_subSampling(void)
{
}
*/

inline DragonflyResources& RES(void *res) { return *(DragonflyResources *)res; }


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


DragonflyDeviceDriver::DragonflyDeviceDriver(void)
{
	system_resources=(void *)new DragonflyResources;
	ACE_ASSERT(system_resources!=NULL);
}

DragonflyDeviceDriver::~DragonflyDeviceDriver()
{
	if (system_resources != NULL)
		delete (DragonflyResources *)system_resources;
	
	system_resources=NULL;
}

///
bool DragonflyDeviceDriver::open (const DragonflyOpenParameters &par)
{
	DragonflyResources& d=RES(system_resources);
	
	return d._initialize(par);
}

bool DragonflyDeviceDriver::close (void)
{
	DragonflyResources& d=RES(system_resources);

	return d._uninitialize();
}

bool DragonflyDeviceDriver::getRawBuffer(unsigned char *buff)
{
    DragonflyResources& d = RES(system_resources);

    return d._capture_raw(buff);
}

bool DragonflyDeviceDriver::getRgbBuffer(unsigned char *buff)
{
    DragonflyResources& d=RES(system_resources);

    return d._capture(buff);
}

bool DragonflyDeviceDriver::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) 
{
    DragonflyResources& d = RES(system_resources);

    //d._capture(d.img);

    image.resize(d.sizeX,d.sizeY);
    
    if(image.getRawImageSize()==d.buffLength) 
    {
        //memcpy(image.getRawImage(),d.img,d.buffLength);
        d._capture(image.getRawImage());
        return true;
    }

    return false;
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
    DragonflyResources& d=RES(system_resources);
    
    return d._setBrightness(value);
}

bool DragonflyDeviceDriver::setShutter(double value)
{
    DragonflyResources& d=RES(system_resources);
    
    return d._setShutter(value);
}

bool DragonflyDeviceDriver::setGain(double value)
{
    DragonflyResources& d=RES(system_resources);
    
    return d._setGain(value);
}

double DragonflyDeviceDriver::getBrightness() const
{
    DragonflyResources& d=RES(system_resources);
    
    return d._getBrightness();
}

double DragonflyDeviceDriver::getShutter() const
{
    DragonflyResources& d=RES(system_resources);
    
    return d._getShutter();
}

double DragonflyDeviceDriver::getGain() const
{
    DragonflyResources& d=RES(system_resources);
    
    return d._getGain();
}

bool DragonflyDeviceDriver::setWhiteBalance(double red, double blue)
{
    DragonflyResources& d=RES(system_resources);
    
    return d._setWhiteBalance(blue,red);
}

bool DragonflyDeviceDriver::getWhiteBalance (double &red, double &blue) const
{
    DragonflyResources& d=RES(system_resources);
    
    return d._getWhiteBalance(blue,red);
}

bool DragonflyDeviceDriver::setAuto(bool bAuto)
{
    DragonflyResources& d=RES(system_resources);
    
    return d._setAuto(bAuto);
}

bool DragonflyDeviceDriver::setAutoBrightness(bool bAuto)
{
    DragonflyResources& d=RES(system_resources);
    
    return d._setAutoBrightness(bAuto);
}

bool DragonflyDeviceDriver::setAutoShutter(bool bAuto)
{
    DragonflyResources& d=RES(system_resources);
    
    return d._setAutoShutter(bAuto);
}

bool DragonflyDeviceDriver::setAutoGain(bool bAuto)
{
    DragonflyResources& d=RES(system_resources);
    
    return d._setAutoGain(bAuto);
}

bool DragonflyDeviceDriver::setAutoWhiteBalance(bool bAuto)
{
    DragonflyResources& d=RES(system_resources);
    
    return d._setAutoWhiteBalance(bAuto);
}

void DragonflyDeviceDriver::PrintSettings()
{
    DragonflyResources& d=RES(system_resources);

    d._printSettings();
}