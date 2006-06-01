// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #emmebi#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: DragonflyDeviceDriver.cpp,v 1.13 2006-06-01 16:20:21 natta Exp $
///
///

#include <yarp/DragonflyDeviceDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <ace/Log_msg.h>

using namespace yarp::dev;
using namespace yarp::os;

//=============================================================================
// FlyCam Includes
//=============================================================================
#include "../dd_orig/include/pgrflycapture.h"

const int _sizeX=640;
const int _sizeY=480;
const int _halfX=_sizeX/2;
const int _halfY=_sizeY/2;

//
class DragonflyResources
{
public:
	DragonflyResources (void) 
	{
		// Variables initialization
		sizeX = _sizeX;
		sizeY = _sizeY;
		maxCams = 0;
		bufIndex = 0;
		_canPost = false;
		imageConverted.pData = NULL;
		_acqStarted = false;
		_validContext = false;
		imageSubSampled = NULL;
        imageFullSize = NULL;
	}

	~DragonflyResources () 
	{ 
		_uninitialize (); // To be sure - must protected against double calling
	}
	
	int sizeX;
	int sizeY;
	int maxCams;
	int bufIndex;
	bool _canPost;
	bool _acqStarted;
	bool _validContext;

	FlyCaptureContext context;
	FlyCaptureImage imageConverted;
	FlyCaptureImage lastBuffer;
	unsigned char *imageSubSampled;
	unsigned char *imageFullSize; //used by the reconstructGenearal method

    //color reconstruction methods
	bool reconstructColor(const unsigned char *src, unsigned char *dst);
	bool recColorFSNN(const unsigned char *src, unsigned char *dst);
    bool recColorFSBilinear(const unsigned char *src, unsigned char *dst);
    bool recColorHSBilinear(const unsigned char *src, unsigned char *dst);
	bool recColorGeneral(const unsigned char *src, unsigned char *dst);
    void subSampling(const unsigned char *src, unsigned char *dst);
    //
    
    bool _initialize (const DragonflyOpenParameters& params);
	bool _uninitialize (void);
	
	bool _setBrightness (int value, bool bDefault=false);
	bool _setExposure (int value, bool bDefault=false);
	bool _setWhiteBalance (int redValue, int blueValue, bool bDefault=false);
	bool _setShutter (int value, bool bDefault=false);
	bool _setGain (int value, bool bDefault=false);

 private:
	void _prepareBuffers (void);
	void _destroyBuffers (void);
};

bool DragonflyResources::reconstructColor(const unsigned char *src, unsigned char *dst)
{
	if ((sizeX == _sizeX) && (sizeY == _sizeY) )
	{
		// full size reconstruction
		recColorFSBilinear(src, dst);
		return true;
	}
    if ((sizeX == _halfX) && (sizeY == _halfY) )
    {
        recColorHSBilinear(src,dst);
	}
	else
    {
	    recColorGeneral(src,dst);
		return true;
    }
    return true;
}

void reportCameraInfo( const FlyCaptureInfoEx* pinfo )
{
   fprintf(stderr, "Serial number: %d\n", pinfo->SerialNumber );
   fprintf(stderr, "Camera model: %s\n", pinfo->pszModelName );
   fprintf(stderr, "Camera vendor: %s\n", pinfo->pszVendorName );
   fprintf(stderr, "Sensor: %s\n", pinfo->pszSensorInfo );
   fprintf(stderr, "DCAM compliance: %1.2f\n", (float)pinfo->iDCAMVer / 100.0 );
   fprintf(stderr, "Bus position: (%d,%d).\n", pinfo->iBusNum, pinfo->iNodeNum );
}

///
///
/// full initialize and startup of the grabber.
inline bool DragonflyResources::_initialize (const DragonflyOpenParameters& params)
{
	FlyCaptureError   error = FLYCAPTURE_OK;

	// LATER: add a camera_init function

	sizeX = params._size_x;
	sizeY = params._size_y;

	// Create the context.
	if (!_validContext)
        {
            error = flycaptureCreateContext( &context );
            if (error != FLYCAPTURE_OK)
                return false;
            _validContext = true;
            // Initialize the camera.
            error = flycaptureInitialize( context, params._unit_number );
            if (error != FLYCAPTURE_OK)
                return false;
        }
	
	FlyCaptureInfoEx info;
	flycaptureGetCameraInfo(context, &info);
	reportCameraInfo( &info );

	// Setup Camera Parameters, Magic Numbers :-)
	_setBrightness(0);
	_setExposure(300);
	_setWhiteBalance(20, 50); 
	_setShutter(320);	// x * 0.0625 = 20 mSec = 50 Hz
	_setGain(500);		// x * -0.0224 = -11.2dB
	
	// Set color reconstruction method
	error = flycaptureSetColorProcessingMethod(context, FLYCAPTURE_NEAREST_NEIGHBOR_FAST); // Should be an Option
	if (error != FLYCAPTURE_OK)
		return false;

	// Set Acquisition Timeout
	error = flycaptureSetGrabTimeoutEx(context, 200);
	if (error != FLYCAPTURE_OK)
		return false;
	// Buffers
	_prepareBuffers ();

	// Start Acquisition
	if (!_acqStarted)
        {
            error = flycaptureStart(	context, 
                                        FLYCAPTURE_VIDEOMODE_640x480Y8,
                                        FLYCAPTURE_FRAMERATE_30);  
            if (error != FLYCAPTURE_OK)
                return false;
            _acqStarted = true;
        }
	
	return true;
}

inline bool DragonflyResources::_uninitialize (void)
{
	FlyCaptureError   error = FLYCAPTURE_OK;

	// Stop Acquisition
	if (_acqStarted)
        {
            error = flycaptureStop(context);
            if (error != FLYCAPTURE_OK)
                return false;
            _acqStarted = false;
        }
	// Destroy the context.
	if (_validContext)
        {
            error = flycaptureDestroyContext( context );
            if (error != FLYCAPTURE_OK)
                return false;
            _validContext = false;
        }
	// Deallocate buffers
	_destroyBuffers();
     
	return true;
}

///
///
inline void DragonflyResources::_prepareBuffers(void)
{
	if (imageConverted.pData == NULL)
		imageConverted.pData = new unsigned char[ _sizeX * _sizeY * 3 ];
	imageConverted.pixelFormat = FLYCAPTURE_BGR;

	if (imageSubSampled == NULL)
		imageSubSampled = new unsigned char[ sizeX * sizeY * 3 ];

    if (imageFullSize == NULL)
        imageFullSize = new unsigned char [_sizeX*_sizeY*3];

	memset(imageSubSampled, 0x0, (sizeX * sizeY * 3));
    memset(imageFullSize, 0x0, (_sizeX*_sizeY*3));
}

inline void DragonflyResources::_destroyBuffers(void)
{
	if (imageConverted.pData != NULL)
		delete [] imageConverted.pData;
	imageConverted.pData = NULL;

    if (imageFullSize != NULL)
        delete [] imageFullSize;
    imageFullSize=NULL;

	if (imageSubSampled != NULL)
		delete [] imageSubSampled;
	imageSubSampled = NULL;
}

inline bool DragonflyResources::_setBrightness (int value, bool bAuto)
{
	FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(context, FLYCAPTURE_BRIGHTNESS, value, 0, bAuto);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

inline bool DragonflyResources::_setExposure (int value, bool bAuto)
{
	FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(context, FLYCAPTURE_AUTO_EXPOSURE, value, 0, bAuto);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

inline bool DragonflyResources::_setWhiteBalance (int redValue, int blueValue, bool bAuto)
{
	FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(context, FLYCAPTURE_WHITE_BALANCE, redValue, blueValue, bAuto);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

inline bool DragonflyResources::_setShutter (int value, bool bAuto)
{
	FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(context, FLYCAPTURE_SHUTTER,value, 0, bAuto);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

inline bool DragonflyResources::_setGain (int value, bool bAuto)
{
	FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(context, FLYCAPTURE_GAIN, value, 0, bAuto);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

inline DragonflyResources& RES(void *res) { return *(DragonflyResources *)res; }

///
///
DragonflyDeviceDriver::DragonflyDeviceDriver(void)
{
    system_resources = 0;
	system_resources = (void *) new DragonflyResources;
	ACE_ASSERT (system_resources != 0);
}

DragonflyDeviceDriver::~DragonflyDeviceDriver()
{
	if (system_resources != 0)
		delete (DragonflyResources *)system_resources;
	system_resources = 0;
}

///
///
bool DragonflyDeviceDriver::open (const DragonflyOpenParameters &par)
{
	DragonflyResources& d = RES(system_resources);
	int ret = d._initialize (par);
    return ret;
}

bool DragonflyDeviceDriver::close (void)
{
	bool ret=true;
	DragonflyResources& d = RES(system_resources);
	ret = d._uninitialize ();
	return ret;
}

bool DragonflyDeviceDriver::getRgbBuffer(unsigned char *buffer)
{
    DragonflyResources& d = RES(system_resources);

    FlyCaptureError error = FLYCAPTURE_OK;

	FlyCaptureImage *pSrc;
	
	pSrc = &(d.lastBuffer);
	
	error = flycaptureGrabImage2(d.context, pSrc);
	if (error!=FLYCAPTURE_OK)
		return false;
	
	d.reconstructColor(pSrc->pData, buffer);

	return true;
}

bool DragonflyDeviceDriver::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    DragonflyResources& d = RES(system_resources);

    FlyCaptureError error = FLYCAPTURE_OK;

	FlyCaptureImage *pSrc;
	
	pSrc = &(d.lastBuffer);
	
	error = flycaptureGrabImage2(d.context, pSrc);
	if (error!=FLYCAPTURE_OK)
		return false;
	
	d.reconstructColor(pSrc->pData, (unsigned char *)image.getRawImage());

	return true;
}

bool DragonflyDeviceDriver::getRawBuffer(unsigned char *buffer)
{
    DragonflyResources& d = RES(system_resources);

    FlyCaptureError error = FLYCAPTURE_OK;

	FlyCaptureImage *pSrc;
	
	pSrc = &(d.lastBuffer);
	
	error = flycaptureGrabImage2(d.context, pSrc);
	if (error!=FLYCAPTURE_OK)
		return false;
	
	memcpy(buffer, pSrc->pData, _sizeX*_sizeY);
	

	return true;
}

int DragonflyDeviceDriver::getRawBufferSize()
{
    DragonflyResources& d = RES(system_resources);

    return _sizeX*_sizeY;
}

int DragonflyDeviceDriver::width () const
{
	return RES(system_resources).sizeX;
}

int DragonflyDeviceDriver::height () const
{
	return RES(system_resources).sizeY;
}

bool DragonflyDeviceDriver::setBrightness(double value)
{
    DragonflyResources& d = RES(system_resources);

    FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(d.context, FLYCAPTURE_BRIGHTNESS, (int) value, 0, false);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

bool DragonflyDeviceDriver::setShutter(double value)
{
    DragonflyResources& d = RES(system_resources);

    FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(d.context, FLYCAPTURE_SHUTTER, (int) value, 0, false);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

bool DragonflyDeviceDriver::setGain(double value)
{
    DragonflyResources& d = RES(system_resources);

    FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(d.context, FLYCAPTURE_GAIN, (int) value, 0, false);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

double DragonflyDeviceDriver::getShutter() const
{
    DragonflyResources& d = RES(system_resources);

    long tmpA;
  
    FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureGetCameraProperty(d.context, FLYCAPTURE_SHUTTER, &tmpA, 0, false);
	if (error == FLYCAPTURE_OK)
		return tmpA;
	else 
		return -1;
}

double DragonflyDeviceDriver::getBrightness() const
{
    DragonflyResources& d = RES(system_resources);

    long tmpA;
    
    FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureGetCameraProperty(d.context, FLYCAPTURE_BRIGHTNESS, &tmpA, 0, false);
	if (error == FLYCAPTURE_OK)
		return tmpA;
	else 
		return -1;
}

double DragonflyDeviceDriver::getGain() const
{
    DragonflyResources& d = RES(system_resources);

    long tmpA;

    FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureGetCameraProperty(d.context, FLYCAPTURE_GAIN, &tmpA, 0, false);
	if (error == FLYCAPTURE_OK)
		return tmpA;
	else 
		return -1;
}

void DragonflyDeviceDriver::recColorFSBilinear(const unsigned char *src, unsigned char *out)
{
    RES(system_resources).recColorFSBilinear(src, out);
}

void DragonflyDeviceDriver::recColorFSNN(const unsigned char *src, unsigned char *out)
{
    RES(system_resources).recColorFSNN(src, out);
}

void DragonflyDeviceDriver::recColorHSBilinear(const unsigned char *src, unsigned char *out)
{
    RES(system_resources).recColorHSBilinear(src, out);
}

////// Reconstruct color methods
// reconstruct color in a full size image, bilinear interpolation
// Assumes pattern: RGRG...RG
//                  GBGB...GB etc..
bool DragonflyResources::recColorFSBilinear(const unsigned char *src, unsigned char *dest)
{
	int tmpB=0.0;
	int tmpG=0.0;
	int tmpR=0.0;

	int rr=0;
	int cc=0;

	unsigned char *tmpSrc=const_cast<unsigned char *>(src);

	///////////// prima riga
	// primo pixel
	tmpG=*(tmpSrc+_sizeX);
	tmpG+=*(tmpSrc+1);

	*dest++=*tmpSrc;
	*dest++=(unsigned char) tmpG/2;
	*dest++=*(tmpSrc+_sizeX+1);
	tmpSrc++;

    // prima riga
	for(cc=1;cc<(_sizeX/2); cc++)
	{
		// first pixel
		tmpR=*(tmpSrc-1);  
		tmpR+=*(tmpSrc+1); 
			
		// x interpolation
		tmpB=*(tmpSrc+_sizeX);
						
		*dest++=(unsigned char)(tmpR/2);
		*dest++=*(tmpSrc);
		*dest++=(unsigned char)(tmpB);

		tmpSrc++;

		// second pixel
        tmpB=*(tmpSrc+_sizeX+1); 
		tmpB+=*(tmpSrc+_sizeX-1); 
			
       	tmpG=*(tmpSrc-1);  
        tmpG+=*(tmpSrc+1); 
		tmpG+=*(tmpSrc+_sizeX); 

		*dest++=*(tmpSrc);
		*dest++=(unsigned char)(tmpG/3);
        *dest++=(unsigned char)(tmpB/2);
		tmpSrc++;
	}

	// last columns, ends with g
	*dest++=*(tmpSrc-1);
	*dest++=*(tmpSrc);
	*dest++=*(tmpSrc+_sizeX);

	tmpSrc++;

	for (rr=1; rr<(_sizeY/2); rr++)
	{
		////////////////// gb row
		// prima colonna
		tmpG=*(tmpSrc);
		tmpB=*(tmpSrc+1);
		tmpR=*(tmpSrc-_sizeX);
		tmpR+=*(tmpSrc+_sizeX);

		*dest++=(unsigned char)(tmpR/2);
		*dest++=tmpG;
		*dest++=tmpB;
		
		tmpSrc++;

		for(cc=1; cc<(_sizeX/2); cc++)
		{
			// second pixel
			tmpR= *(tmpSrc-_sizeX-1);  
			tmpR+= *(tmpSrc-_sizeX+1); 
			tmpR+= *(tmpSrc+_sizeX-1); 
			tmpR+= *(tmpSrc+_sizeX+1); 
			
			// + interpolation
			tmpG=*(tmpSrc-_sizeX);		
			tmpG+=*(tmpSrc-1);	
			tmpG+=*(tmpSrc+1);		
			tmpG+=*(tmpSrc+_sizeX);		

			*dest++=(unsigned char)(tmpR/4);
			*dest++=(unsigned char)(tmpG/4);
			*dest++=*tmpSrc;

            tmpSrc++;

			// first pixel
			tmpR=*(tmpSrc-_sizeX);	
			tmpR+=*(tmpSrc+_sizeX);
			
			// x interpolation
			tmpB=*(tmpSrc-1);
			tmpB+=*(tmpSrc+1);
						
			*dest++=(unsigned char)(tmpR/2);
			*dest++=*tmpSrc;
			*dest++=(unsigned char)(tmpB/2);
			
            tmpSrc++;
		}
		//last col, ends with b
		*dest++=*(tmpSrc+_sizeX-1);
		*dest++=*(tmpSrc+_sizeX);
		*dest++=*tmpSrc;

		tmpSrc++;
			
		////////////////// gb row
		// prima colonna
		tmpG=*(tmpSrc-_sizeX);	
		tmpG+=*(tmpSrc+_sizeX);	
		tmpG+=*(tmpSrc+1);	

		tmpB=*(tmpSrc-_sizeX+1);
		tmpB+=*(tmpSrc+_sizeX+1);
		tmpR=*tmpSrc;
		
		*dest++=(unsigned char)(tmpR);
		*dest++=(unsigned char)(tmpG/3);
		*dest++=(unsigned char)(tmpB/2);

		tmpSrc++;

		// altre colonne
		for(cc=1; cc<(_sizeX/2); cc++)
		{
			// second pixel
			tmpB=*(tmpSrc-_sizeX);	
			tmpB+=*(tmpSrc+_sizeX);  
			
			// x interpolation
			tmpR=*(tmpSrc-1);	
			tmpR+=*(tmpSrc+1);	
						
			*dest++=(unsigned char)(tmpR/2);
			*dest++=*tmpSrc;
			*dest++=(unsigned char)(tmpB/2);

            tmpSrc++;

			// first pixel, x interpolation
			tmpB=*(tmpSrc-_sizeX-1);		
			tmpB+=*(tmpSrc-_sizeX+1);	
			tmpB+=*(tmpSrc+_sizeX-1);
			tmpB+=*(tmpSrc+_sizeX+1);	
			
			// + interpolation
			tmpG=*(tmpSrc+1);
			tmpG+=*(tmpSrc-1);
			tmpG+=*(tmpSrc+_sizeX);
			tmpG+=*(tmpSrc-_sizeX);
			
			*dest++=*tmpSrc;
			*dest++=(unsigned char)(tmpG/4);
			*dest++=(unsigned char)(tmpB/4);
			
            tmpSrc++;
		}
	
		*dest++=*(tmpSrc-1);
		*dest++=*(tmpSrc);
		*dest++=*(tmpSrc-_sizeX);

		tmpSrc++;
	}

	//////////// ultima riga
	// prima colonna
	*dest++=*(tmpSrc-_sizeX);
	*dest++=*(tmpSrc);
	*dest++=*(tmpSrc+1);

	tmpSrc++;

	for(cc=1;cc<=(sizeX/2-1); cc++)
	{
		*dest++=*(tmpSrc-_sizeX+1);
		*dest++=*(tmpSrc+1);
		*dest++=*(tmpSrc);
		tmpSrc++;

        *dest++=*(tmpSrc-_sizeX);
		*dest++=*(tmpSrc);
		*dest++=*(tmpSrc-_sizeX+1);
		tmpSrc++;
	}

	// ultimo pixel
	*dest++=*(tmpSrc-1-_sizeX);
	*dest++=*(tmpSrc-1);
	*dest++=*tmpSrc;
	tmpSrc++;

	return true;
}

// reconstruct color in a full size image, nearest neighbor interpolation
// Assumes pattern: RGRG...RG
//                  GBGB...GB etc..
bool DragonflyResources::recColorFSNN(const unsigned char *src, unsigned char *dest)
{
	int tmpB=0.0;
	int tmpG=0.0;
	int tmpR=0.0;

	int rr=0;
	int cc=0;

	unsigned char *tmpSrc=const_cast<unsigned char *>(src);

	///////////// prima riga
	// primo pixel
	*dest++=*tmpSrc;
	*dest++=*(tmpSrc+1);
	*dest++=*(tmpSrc+_sizeX+1);
	tmpSrc++;

    // prima riga
	for(cc=1;cc<(_sizeX/2); cc++)
	{
		// first pixel
		*dest++=*(tmpSrc+1);
		*dest++=*(tmpSrc);
		*dest++=*(tmpSrc+_sizeX);

		tmpSrc++;

		// second pixel
		*dest++=*(tmpSrc);
		*dest++=*(tmpSrc+1);
        *dest++=*(tmpSrc+_sizeX-1);

        tmpSrc++;
	}

	// last columns, ends with g
	*dest++=*(tmpSrc-1);
	*dest++=*(tmpSrc);
	*dest++=*(tmpSrc+_sizeX);

	tmpSrc++;

	for (rr=1; rr<=(_sizeY/2-1); rr++)
	{
		////////////////// gb row
		// prima colonna
		*dest++=*(tmpSrc+_sizeX);
		*dest++=*(tmpSrc+1);
		*dest++=*(tmpSrc);
		
		tmpSrc++;

		for(cc=1; cc<(_sizeX/2); cc++)
		{
			// second pixel
			*dest++=*(tmpSrc+_sizeX+1);
			*dest++=*(tmpSrc+1);
            *dest++=*tmpSrc;

            tmpSrc++;

			// first pixel
			*dest++=*(tmpSrc+_sizeX);
			*dest++=*tmpSrc;
			*dest++=*(tmpSrc+1);

            tmpSrc++;
		}
		//last col, ends with b
		*dest++=*(tmpSrc+_sizeX-1);
		*dest++=*(tmpSrc+_sizeX);
		*dest++=*tmpSrc;

		tmpSrc++;
			
		////////////////// gb row
		// prima colonna
		*dest++=*tmpSrc;
		*dest++=*(tmpSrc+1);
		*dest++=*(tmpSrc+_sizeX+1);

		tmpSrc++;

		// altre colonne
		for(cc=1; cc<(_sizeX/2); cc++)
		{
			// second pixel
			*dest++=*(tmpSrc+1);
			*dest++=*tmpSrc;
			*dest++=*(tmpSrc+_sizeX);

            tmpSrc++;

			// first pixel, x interpolation
			*dest++=*tmpSrc;
			*dest++=*(tmpSrc+1);
			*dest++=*(tmpSrc+_sizeX+1);

            tmpSrc++;
		}
	
		*dest++=*(tmpSrc-1);
		*dest++=*(tmpSrc);
		*dest++=*(tmpSrc-_sizeX);

		tmpSrc++;
	}

	//////////// ultima riga
	// prima colonna
	*dest++=*(tmpSrc-_sizeX);
	*dest++=*(tmpSrc);
	*dest++=*(tmpSrc+1);

	tmpSrc++;

	for(cc=1;cc<=(sizeX/2-1); cc++)
	{
		*dest++=*(tmpSrc-_sizeX+1);
		*dest++=*(tmpSrc+1);
		*dest++=*(tmpSrc);
		tmpSrc++;

        *dest++=*(tmpSrc-_sizeX);
		*dest++=*(tmpSrc);
		*dest++=*(tmpSrc-_sizeX+1);
		tmpSrc++;
	}

	// ultimo pixel
	*dest++=*(tmpSrc-1-_sizeX);
	*dest++=*(tmpSrc-1);
	*dest++=*tmpSrc;
	tmpSrc++;

	return true;
}

////// Reconstruct color methods
// reconstruct color in a full size image, bilinear interpolation
// Assumes pattern: RGRG...RG
//                  GBGB...GB etc..
bool DragonflyResources::recColorHSBilinear(const unsigned char *src, unsigned char *dest)
{
	int tmpB=0.0;
	int tmpG=0.0;
	int tmpR=0.0;

	int rr=0;
	int cc=0;

    int r=0;
    int c=0;

	unsigned char *tmpSrc=const_cast<unsigned char *>(src);

    //////// prima riga
	// primo pixel
	*dest++=*(tmpSrc);
	*dest++=*(tmpSrc+1);
	*dest++=*(tmpSrc+_sizeX+1);
	tmpSrc+=2;

    c++;

    // prima riga
	for(cc=1;cc<(_halfX-1); cc++)
	{
		// first pixel
		tmpB=*(tmpSrc+_sizeX+1);
        tmpB+=*(tmpSrc+_sizeX-1);
        
        tmpG=*(tmpSrc-1);
        tmpG+=*(tmpSrc+1);
        tmpG+=*(tmpSrc+_sizeX);
						
		*dest++=*(tmpSrc);
		*dest++=(unsigned char)(tmpG/3);
		*dest++=(unsigned char)(tmpB/2);

		tmpSrc+=2;

        c++;
	}

  	// last columns, ends with r
	*dest++=*(tmpSrc);
	*dest++=*(tmpSrc-1);
	*dest++=*(tmpSrc+_sizeX-1);
	tmpSrc+=2;
    
    c++;

    // skip a row
    tmpSrc+=_sizeX;

    c=0;
    r++;
	for (rr=1; rr<(_halfY-1); rr++)
	{
		////////////////// rg row
		// prima colonna
		*dest++=*tmpSrc;
		*dest++=*(tmpSrc+1);
		*dest++=*(tmpSrc+_sizeX+1);
		
		tmpSrc+=2;
        c++;

		for(cc=1; cc<(_halfX-1); cc++)
		{
            // first pixel
            tmpB=*(tmpSrc+_sizeX+1);
            tmpB+=*(tmpSrc+_sizeX-1);
            tmpB+=*(tmpSrc-_sizeX+1);
            tmpB+=*(tmpSrc-_sizeX-1);
            
            tmpG=*(tmpSrc-1);
            tmpG+=*(tmpSrc+1);
            tmpG+=*(tmpSrc+_sizeX);
            tmpG+=*(tmpSrc-_sizeX);
            
            *dest++=*(tmpSrc);
            *dest++=(unsigned char)(tmpG/4);
            *dest++=(unsigned char)(tmpB/4);
            
            tmpSrc+=2;
            c++;
		}
		//last col, ends with r
 	    *dest++=*(tmpSrc);
	    *dest++=*(tmpSrc-1);
	    *dest++=*(tmpSrc+_sizeX-1);
 
		tmpSrc+=2;
        tmpSrc+=_sizeX; //skip a row

        r++;
        c=0;
    }

    c=0;

    //////// ultima riga
	// primo pixel
	*dest++=*(tmpSrc);
	*dest++=*(tmpSrc+1);
	*dest++=*(tmpSrc-_sizeX+1);
	tmpSrc+=2;

    c++;

    for(cc=1;cc<(_halfX-1); cc++)
	{
		// first pixel
		tmpB=*(tmpSrc-_sizeX+1);
        tmpB+=*(tmpSrc-_sizeX-1);
        
        tmpG=*(tmpSrc-1);
        tmpG+=*(tmpSrc+1);
        tmpG+=*(tmpSrc-_sizeX);
						
		*dest++=*(tmpSrc);
		*dest++=(unsigned char)(tmpG/3);
		*dest++=(unsigned char)(tmpB/2);

		tmpSrc+=2;

        c++;
	}

  	// last columns, ends with r
	*dest++=*(tmpSrc);
	*dest++=*(tmpSrc-1);
	*dest++=*(tmpSrc-_sizeX-1);
	
	return true;
}

////// Reconstruct color and downsampling, general case.
// Assumes pattern: RGRG...RG
//                  GBGB...GB etc..
bool DragonflyResources::recColorGeneral(const unsigned char *src, unsigned char *dest)
{
    bool ret;
    ret=recColorFSNN(src, imageFullSize);
    subSampling(imageFullSize, dest);
    return ret;
}

void DragonflyResources::subSampling(const unsigned char *src, unsigned char *dest)
{
	int srcX, srcY;
	float xRatio, yRatio;
	int srcOffset;

	int srcSizeY = _sizeY, srcSizeX = _sizeX;
	int dstSizeY = sizeY, dstSizeX = sizeX;
	int bytePerPixel = 3;

	xRatio = ((float)srcSizeX)/dstSizeX;
	yRatio = ((float)srcSizeY)/dstSizeY;

    const unsigned char *pSrc = src;
	unsigned char *pDst = dest;

	for (int j=0; j<dstSizeY; j++)
        {
            srcY = (int)(yRatio*j);
            srcOffset = srcY * srcSizeX * bytePerPixel;

            for (int i=0; i<dstSizeX; i++)
                {
                    srcX = (int)(xRatio*i);
                    pSrc = src + srcOffset + (srcX * bytePerPixel);
                    memcpy(pDst,pSrc,bytePerPixel);
                    pDst += bytePerPixel;

                }
        }
}

