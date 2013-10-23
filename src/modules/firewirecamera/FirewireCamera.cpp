// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <FirewireCamera.h>

#include <yarp/os/Semaphore.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>

#include "FirewireCameraSet.h"

using namespace yarp::os;
using namespace yarp::dev;

// Most of this is not used (here because the driver was ported).
class FirewireCameraResources
{
public:
    FirewireCameraResources (void)
    {
    }

    ~FirewireCameraResources ()
    {
        _uninitialize();
    }


    // Hardware-dependant variables
    enum { _num_buffers = 3 };

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

    inline bool _initialize (const FirewireCameraOpenParameters & params);
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

CFWCameraSet* FirewireCameraResources::m_pCameraSet=NULL;
Semaphore FirewireCameraResources::m_InitCloseMutex;

/// full initialize and startup of the grabber.
inline bool FirewireCameraResources::_initialize (const FirewireCameraOpenParameters& params)
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
        ACE_OS::fprintf(stderr, "FirewirecameraResources: can't open camera %d",unit_number);
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

inline bool FirewireCameraResources::_uninitialize()
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

inline bool FirewireCameraResources::_setBrightness(double value)
{
    return m_pCameraSet->SetBrightness(unit_number,value);
}

inline bool FirewireCameraResources::_setWhiteBalance(double blue, double red)
{
    return m_pCameraSet->SetWhiteBalance(unit_number,blue,red);
}

inline bool FirewireCameraResources::_setShutter(double value)
{
    return m_pCameraSet->SetShutter(unit_number,value);
}

inline bool FirewireCameraResources::_setGain(double value)
{
    return m_pCameraSet->SetGain(unit_number,value);
}

inline double FirewireCameraResources::_getBrightness()
{
    return m_pCameraSet->GetBrightness(unit_number);
}

inline bool FirewireCameraResources::_getWhiteBalance(double& blue, double& red)
{
    return m_pCameraSet->GetWhiteBalance(unit_number,blue,red);
}

inline double FirewireCameraResources::_getShutter()
{
    return m_pCameraSet->GetShutter(unit_number);
}

inline double FirewireCameraResources::_getGain()
{
    return m_pCameraSet->GetGain(unit_number);
}

inline bool FirewireCameraResources::_setAuto(bool bAuto)
{
    bool bOk=true;

    bOk=bOk && _setAutoBrightness(bAuto);
    bOk=bOk && _setAutoWhiteBalance(bAuto);
    bOk=bOk && _setAutoShutter(bAuto);
    bOk=bOk && _setAutoGain(bAuto);

    return bOk;
}

inline bool FirewireCameraResources::_setAutoBrightness(bool bAuto)
{
    return m_pCameraSet->SetAutoBrightness(unit_number,bAuto);
}

inline bool FirewireCameraResources::_setAutoWhiteBalance(bool bAuto)
{
    return m_pCameraSet->SetAutoWhiteBalance(unit_number,bAuto);
}

inline bool FirewireCameraResources::_setAutoShutter(bool bAuto)
{
    return m_pCameraSet->SetAutoShutter(unit_number,bAuto);
}

inline bool FirewireCameraResources::_setAutoGain(bool bAuto)
{
    return m_pCameraSet->SetAutoGain(unit_number,bAuto);
}

/*
inline void DragonflyResources::_subSampling(void)
{
}
*/

inline FirewireCameraResources& RES(void *res) { return *(FirewireCameraResources *)res; }


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


FirewireCamera::FirewireCamera(void)
{
    system_resources=(void *)new FirewireCameraResources;
    ACE_ASSERT(system_resources!=NULL);
}

FirewireCamera::~FirewireCamera()
{
    if (system_resources != NULL)
        delete (FirewireCameraResources *)system_resources;

    system_resources=NULL;
}

///
bool FirewireCamera::open (const FirewireCameraOpenParameters &par)
{
    FirewireCameraResources& d=RES(system_resources);

    return d._initialize(par);
}

bool FirewireCamera::close (void)
{
    FirewireCameraResources& d=RES(system_resources);

    return d._uninitialize();
}

bool FirewireCamera::getRawBuffer(unsigned char *buff)
{
    FirewireCameraResources& d = RES(system_resources);

    return d._capture_raw(buff);
}

bool FirewireCamera::getRgbBuffer(unsigned char *buff)
{
    FirewireCameraResources& d=RES(system_resources);

    return d._capture(buff);
}

bool FirewireCamera::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    FirewireCameraResources& d = RES(system_resources);

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

int FirewireCamera::getRawBufferSize()
{
    FirewireCameraResources& d = RES(system_resources);
    return d.buffLength;
}

int FirewireCamera::width () const
{
    return RES(system_resources).sizeX;
}

int FirewireCamera::height () const
{
    return RES(system_resources).sizeY;
}

bool FirewireCamera::setBrightness (double value)
{
    FirewireCameraResources& d=RES(system_resources);

    return d._setBrightness(value);
}

bool FirewireCamera::setShutter(double value)
{
    FirewireCameraResources& d=RES(system_resources);

    return d._setShutter(value);
}

bool FirewireCamera::setGain(double value)
{
    FirewireCameraResources& d=RES(system_resources);

    return d._setGain(value);
}

double FirewireCamera::getBrightness() const
{
    FirewireCameraResources& d=RES(system_resources);

    return d._getBrightness();
}

double FirewireCamera::getShutter() const
{
    FirewireCameraResources& d=RES(system_resources);

    return d._getShutter();
}

double FirewireCamera::getGain() const
{
    FirewireCameraResources& d=RES(system_resources);

    return d._getGain();
}

bool FirewireCamera::setWhiteBalance(double red, double blue)
{
    FirewireCameraResources& d=RES(system_resources);

    return d._setWhiteBalance(blue,red);
}

bool FirewireCamera::getWhiteBalance (double &red, double &blue) const
{
    FirewireCameraResources& d=RES(system_resources);

    return d._getWhiteBalance(blue,red);
}

bool FirewireCamera::setAuto(bool bAuto)
{
    FirewireCameraResources& d=RES(system_resources);

    return d._setAuto(bAuto);
}

bool FirewireCamera::setAutoBrightness(bool bAuto)
{
    FirewireCameraResources& d=RES(system_resources);

    return d._setAutoBrightness(bAuto);
}

bool FirewireCamera::setAutoShutter(bool bAuto)
{
    FirewireCameraResources& d=RES(system_resources);

    return d._setAutoShutter(bAuto);
}

bool FirewireCamera::setAutoGain(bool bAuto)
{
    FirewireCameraResources& d=RES(system_resources);

    return d._setAutoGain(bAuto);
}

bool FirewireCamera::setAutoWhiteBalance(bool bAuto)
{
    FirewireCameraResources& d=RES(system_resources);

    return d._setAutoWhiteBalance(bAuto);
}

void FirewireCamera::PrintSettings()
{
    FirewireCameraResources& d=RES(system_resources);

    d._printSettings();
}
