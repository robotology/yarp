// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Claudio Castellini, Mattia Castelnovi
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/*
 * Copyright (C) 2006 Giorgio Metta, Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "yarp/PicoloDeviceDriver.h"

#include <ace/os.h>
#include <ace/Sched_Params.h>
#include <ace/Log_Msg.h>

#include <yarp/os/Semaphore.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>

#ifndef _WINDOWS
#define _WINDOWS
#endif

#include "../dd_orig/include/Picolo32.h"

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

class PicoloResources : public Thread {
public:

	PicoloResources (void) : _bmutex(1), _new_frame(0) {
		_nRequestedSizeX = 0;
		_nRequestedSizeY = 0;
		_nWidth = 0;
		_nHeight = 0;
		_nImageSize = 0;
		_picoloHandle = 0;

		memset (_bufHandles, 0, sizeof(PICOLOHANDLE) * _num_buffers);
		memset (_buffer, 0, sizeof(PUINT8) * _num_buffers);
		memset (_aligned, 0, sizeof(PUINT8) * _num_buffers);

		_rawBuffer = NULL;
		_canpost = true;
	}

	~PicoloResources () { _uninitialize (); }

	enum { _num_buffers = 3 };

	PICOLOHANDLE _picoloHandle;	

	Semaphore _bmutex;
	Semaphore _new_frame;

	// Img size are determined partially by the HW.
	UINT32 _nRequestedSizeX;
	UINT32 _nRequestedSizeY;
	UINT32 _nWidth;
	UINT32 _nHeight;
	UINT32 _nImageSize;

	PICOLOHANDLE _bufHandles[_num_buffers];
	PUINT8 _buffer[_num_buffers];
	PUINT8 _aligned[_num_buffers];

	bool _canpost;

	unsigned char *_rawBuffer;

	inline bool _initialize (const PicoloOpenParameters& params);
	inline bool _uninitialize (void);

    //thread body
    virtual void run(void);

    void setPriority(int p) {
        fprintf(stderr, "Asked to set priority, doing nothing.\n");
    }

    int getPriority() {
        fprintf(stderr, "Asked to get priority, doing nothing.\n");
        return 0;
    }

protected:
	inline PICOLOHANDLE _init (const PicoloOpenParameters& params);
	inline void _prepareBuffers (void);

};

// full initialize and startup of the grabber.

inline bool PicoloResources::_initialize (const PicoloOpenParameters& params)
{

	if ( _init (params) == false ) return false;
	_prepareBuffers ();

	// n-part buffering.
	PICOLOHANDLE BufferList[_num_buffers];
	PICOLOSTATUS PicoloStatus;

	int i;
	for (i = 0; i < _num_buffers; i++) {
        BufferList[i] = _bufHandles[i];
    }

	PicoloStatus = PicoloSetBufferList (_picoloHandle, BufferList, _num_buffers);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// select initial buffer.
	PicoloStatus = PicoloSelectBuffer(_picoloHandle, 0);

	// starts continuous acquisition.
	PicoloStatus = PicoloAcquire (_picoloHandle, PICOLO_ACQUIRE_CONTINUOUS | PICOLO_ACQUIRE_INC, 1);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	return true;

}

inline bool PicoloResources::_uninitialize (void)
{

	_bmutex.wait ();

	if (_nRequestedSizeX == 0 && _nRequestedSizeY == 0 && _nWidth == 0 && _nHeight == 0)
		return false;

	PicoloAcquire (_picoloHandle, PICOLO_ACQUIRE_STOP, 1);
	PicoloStop (_picoloHandle);

	int i;
	for (i = 0; i < _num_buffers; i++)
		VirtualFree (_buffer[i], _nImageSize, MEM_DECOMMIT);

	if (_rawBuffer != NULL) delete[] _rawBuffer;
	_rawBuffer = NULL;

	_nRequestedSizeX = 0;
	_nRequestedSizeY = 0;
	_nWidth = 0;
	_nHeight = 0;
	_nImageSize = 0;
	_picoloHandle = 0;
	_canpost = true;
	
	_bmutex.post();

	return true;

}

inline PICOLOHANDLE PicoloResources::_init (const PicoloOpenParameters& params)
{

	/// copy params.
	_nRequestedSizeX = params._size_x;
	_nRequestedSizeY = params._size_y;

	/// starts board up.
	PICOLOHANDLE ret = PicoloStart(params._unit_number);
	if (ret < 0) {
        ACE_DEBUG ((LM_DEBUG, "troubles opening the grabber number %d\n", params._unit_number));
        return false;
    }

	// video input.
	PICOLOSTATUS PicoloStatus;
	ACE_ASSERT (params._video_type == 0 || params._video_type == 1);

	/// it might require more params (e.g. ntsc, etc.)
	if (params._video_type == 0) {
        PicoloStatus = PicoloSelectVideoInput(ret, PICOLO_INPUT_COMPOSITE_BNC, PICOLO_IFORM_STD_PAL);
    } else {
        PicoloStatus = PicoloSelectVideoInput(ret, PICOLO_INPUT_SVIDEO_MINIDIN4, PICOLO_IFORM_625LINES);
    }

	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// image type. Color only?
	PicoloStatus = PicoloSelectImageFormat(ret, PICOLO_COLOR_RGB24);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// assume we want a square image
	float scalex = float(768.0/_nRequestedSizeX);
	float scaley = (float) (576.0/(_nRequestedSizeY*params._alfa));	// a slighlty bigger immage is acquired, this allows some offsets along the vertical direction (see offset)
	float scale = (scalex < scaley) ? scalex : scaley;
	scalex = scale;
	scaley = scale / 2.0f;	//the image is interlaced
	float xSize = 768.0f/scalex;
	float ySize = 576.0f/scaley;
	int offsetX = (int) ((xSize-_nRequestedSizeX) / 2 + params._offset_x + 0.5);
	int offsetY = (int) ((ySize-_nRequestedSizeY*2.0) / 2 + params._offset_y + 0.5);
	
	// adjust size and scaling. 
	PicoloStatus = PicoloSetControlFloat(ret, PICOLO_CID_ADJUST_SCALEX, scalex);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// img height is twice the requested size.
	PicoloStatus = PicoloSetControlFloat(ret, PICOLO_CID_ADJUST_SCALEY, scaley);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);
	PicoloStatus = PicoloSetControlValue(ret, PICOLO_CID_ADJUST_SIZEX, _nRequestedSizeX);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);
	PicoloStatus = PicoloSetControlValue(ret, PICOLO_CID_ADJUST_SIZEY, 2*_nRequestedSizeY);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	PicoloStatus = PicoloSetControlValue(ret, PICOLO_CID_ADJUST_OFFSETX, offsetX);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);
	PicoloStatus = PicoloSetControlValue(ret, PICOLO_CID_ADJUST_OFFSETY, offsetY);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// re-get size (to be sure!)
	PicoloStatus = PicoloGetImageSize(ret, &_nWidth, &_nHeight);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	PicoloStatus = PicoloGetImageBufferSize(ret, &_nImageSize); 
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	_bmutex.wait ();
	_rawBuffer = new unsigned char [_nImageSize];
	ACE_ASSERT (_rawBuffer != NULL);
	_bmutex.post ();

	/// all ok, store the handle.
	_picoloHandle = ret;

	return ret;

}

void PicoloResources::run (void)
{

	PICOLOSTATUS PicoloStatus;

	const int prio = ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, getPriority(), ACE_SCOPE_THREAD);
	setPriority (prio);

	PicoloStatus = PicoloSetWaitTimeout (_picoloHandle, 500);		/// timeout 120 ms.

	_canpost = true;

	int i = 0;

	unsigned int bufno = 0;
	PicoloStatus = PicoloGetCurrentBuffer (_picoloHandle, &bufno);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);
	const unsigned int startbuf = (bufno > 0) ? (bufno - 1) : (_num_buffers - 1);
	unsigned int readfro = startbuf;

	/// strategy, waits, copy into lockable buffer.
	while ( ! isStopping() ) {
        PicoloStatus = PicoloWaitEvent (_picoloHandle, PICOLO_EV_END_ACQUISITION);
        if (PicoloStatus != PICOLO_OK) {
            ACE_DEBUG ((LM_DEBUG, "it's likely that the acquisition timed out, returning\n"));
            ACE_DEBUG ((LM_DEBUG, "WARNING: this leaves the acquisition thread in an unterminated state --- can't be restarted from here!\n"));
            break; //we want to leave the while loop, and hopefully end the thread 
        }

        readfro = startbuf;

        for (i = 0; i < _num_buffers; i++) {
            if (_bmutex.check () == true) {
                // buffer acquired.
                // read from buffer
                memcpy (_rawBuffer, _aligned[readfro], _nImageSize);
                if (_canpost) {
                    _canpost = false;
                    _new_frame.post();
                }
                _bmutex.post();
			} else {
                // can't acquire, it means the buffer is still in use.
                // silently ignores this condition.
                ACE_DEBUG ((LM_DEBUG, "lost a frame, acq thread\n"));
            }

            readfro = ((readfro + 1) % _num_buffers);

            // 40 ms delay
            if (i < _num_buffers-1)
//                Time::delay (0.040);
                Time::delay (0.020);
        }
    }

	ACE_DEBUG ((LM_DEBUG, "acquisition thread returning...\n"));

}

inline void PicoloResources::_prepareBuffers(void)
{

	int i;
	for (i = 0; i < _num_buffers; i++) {
        _buffer[i] = (PUINT8)VirtualAlloc(NULL, _nImageSize, MEM_COMMIT, PAGE_READWRITE);
        ACE_ASSERT (_buffer[i] != NULL);
        _bufHandles[i] = PicoloSetImageBuffer(_picoloHandle, _buffer[i], _nImageSize, PICOLO_FIELD_DOWN_ONLY, (PVOID*) &_aligned[i]);
        ACE_ASSERT (_bufHandles[i] >= 0);
        memset(_aligned[i], 0, _nImageSize);
    }

}

inline PicoloResources& RES(void *res) { return *(PicoloResources *)res; }

PicoloDeviceDriver::PicoloDeviceDriver()
{

	system_resources = (void *) new PicoloResources;
	ACE_ASSERT (system_resources != NULL);

}

PicoloDeviceDriver::~PicoloDeviceDriver()
{

	if (system_resources != NULL)
		delete (PicoloResources *)system_resources;
	system_resources = NULL;

}

bool PicoloDeviceDriver::open(yarp::os::Searchable& config)
{

	PicoloOpenParameters par;
	PicoloResources& d = RES(system_resources);
	
	par._size_x = config.find("width").asInt();
	par._size_y = config.find("height").asInt();
	par._offset_y = config.find("yoffset").asInt();
	par._unit_number = config.find("unit").asInt();

    if ( d._initialize (par) == false ) 
        return false;

	d.start();

	return true;

}


bool PicoloDeviceDriver::close (void)
{

	PicoloResources& d = RES(system_resources);

	d.stop();

	return d._uninitialize ();

}

bool PicoloDeviceDriver::getRgbBuffer(unsigned char *buff)
{
    
	PicoloResources& d = RES(system_resources);

    char *tmpBuff;
    waitOnNewFrame ();
	acquireBuffer(&tmpBuff);

	memcpy(buff, tmpBuff, d._nRequestedSizeX * d._nRequestedSizeY * 3);

	releaseBuffer ();

    return true;

}

bool PicoloDeviceDriver::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{

    PicoloResources& d = RES(system_resources);

    FlexImage flex;
    flex.setQuantum(1);
    flex.setPixelCode(VOCAB_PIXEL_RGB);
    flex.resize(d._nRequestedSizeX,d._nRequestedSizeY);

    char *tmpBuff;
    waitOnNewFrame ();
	acquireBuffer(&tmpBuff);

	memcpy(flex.getRawImage(), tmpBuff, flex.getRawImageSize());

	releaseBuffer ();

    image.copy(flex);

    return true;

}

bool PicoloDeviceDriver::getRawBuffer(unsigned char *buff)
{
    
	PicoloResources& d = RES(system_resources);

    char *tmpBuff;
    waitOnNewFrame ();
	acquireBuffer(&tmpBuff);

	memcpy(buff, tmpBuff, d._nRequestedSizeX * d._nRequestedSizeY * 3);

	releaseBuffer();

    return true;

}


int PicoloDeviceDriver::getRawBufferSize()
{

	PicoloResources& d = RES(system_resources);

	return	d._nRequestedSizeX * d._nRequestedSizeY * 3;

}

bool PicoloDeviceDriver::acquireBuffer (void *buffer)
{

	PicoloResources& d = RES(system_resources);
	d._bmutex.wait ();
	(*(unsigned char **)buffer) = d._rawBuffer;

	return true;

}

bool PicoloDeviceDriver::releaseBuffer ()
{
	
	PicoloResources& d = RES(system_resources);
	d._canpost = true;
	d._bmutex.post ();

	return true;

}

bool PicoloDeviceDriver::waitOnNewFrame ()
{
	
	PicoloResources& d = RES(system_resources);
	d._new_frame.wait();

	return true;

}

int PicoloDeviceDriver::width () const
{

    PicoloResources& d = RES(system_resources);
    
	return d._nRequestedSizeX;

}

int PicoloDeviceDriver::height () const
{
    
	return RES(system_resources)._nRequestedSizeY;

}

#if 0
int PicoloDeviceDriver::setBrightness (void *cmd)
{
	/// RES(system_resources)._setBrightness(*cmd);
	return true;
}

int PicoloDeviceDriver::setHue (void *cmd)
{
	/// RES(system_resources)._setHue(*cmd);
	return true;
}

int PicoloDeviceDriver::setContrast (void *cmd)
{
	/// RES(system_resources)._setContrast(*cmd);
	return true;
}

int PicoloDeviceDriver::setSatU (void *cmd)
{
	/// RES(system_resources)._setSatU(*cmd);
	return true;
}

int PicoloDeviceDriver::setSatV (void *cmd)
{
	/// RES(system_resources)._setSatV(*cmd);
	return true;
}

int PicoloDeviceDriver::setLNotch (void *cmd)
{
	/// RES(system_resources)._setLNotch(*cmd);
	return true;
}

int PicoloDeviceDriver::setLDec (void *cmd)
{
	/// RES(system_resources)._setLDec(*cmd);
	return true;
}

int PicoloDeviceDriver::setCrush (void *cmd)
{
	/// RES(system_resources)._setCrush(*cmd);
	return true;
}
#endif

#undef _WINDOWS
