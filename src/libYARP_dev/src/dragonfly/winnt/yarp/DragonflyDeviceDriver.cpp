// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2006 Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/

///
/// $Id: DragonflyDeviceDriver.cpp,v 1.29 2007-04-13 17:18:15 babybot Exp $
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

//forward function declaration
bool Bayer2RGB( const unsigned char* bayer0, int bayer_step, unsigned char *dst0, int dst_step, int width, int height, int blue, int start_with_green, int rgb );

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
	bool fleaCR;

	double m_Gain,m_Shutter,m_Brightness,m_WhiteB,m_WhiteR;
	int min_Brightness,min_Shutter,min_Gain,min_RBbalance;
	int max_Brightness,max_Shutter,max_Gain,max_RBbalance;

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

	bool _setBrightness (double value);
	bool _setWhiteBalance (double blueValue, double redValue);
	bool _setShutter (double value);
	bool _setGain (double value);

	double _getShutter() const;
	double _getBrightness() const;
	double _getGain() const;
	bool _getWhiteBalance (double &blueValue, double &redValue) const;

	bool _setAutoBrightness(bool bAuto);
	bool _setAutoShutter(bool bAuto);
	bool _setAutoGain(bool bAuto);
	bool _setAutoWhiteBalance(bool bAuto);

private:
	void _prepareBuffers (void);
	void _destroyBuffers (void);
};

bool DragonflyResources::reconstructColor(const unsigned char *src, unsigned char *dst)
{
	//for flea cameras - not optimized for subsampling
	if(fleaCR)
	{
		if ((sizeX == _sizeX) && (sizeY == _sizeY) )
		{
			// full size reconstruction
			Bayer2RGB( src, sizeX, dst, sizeX*3, sizeX, sizeY, -1, 1, 1 );
			return true;
		}
		else
		{
			Bayer2RGB( src, _sizeX, imageFullSize, _sizeX*3, _sizeX, _sizeY, -1, 1, 1 );
			//memcpy(imageFullSize, dst, sizeX*sizeY*3);
			subSampling(imageFullSize, dst);
			return true;
		}
	}

	//for dragonfly cameras
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
	fleaCR = params._fleacr;

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

	// get control value ranges for normalization purposes
	error = flycaptureGetCameraPropertyRangeEx(	context, 
		FLYCAPTURE_BRIGHTNESS, 
		NULL, NULL, NULL, NULL, NULL, NULL,
		&min_Brightness,
		&max_Brightness);

	if (error == FLYCAPTURE_OK)
	{
		printf("Brightness range [%d : %d]\n",min_Brightness,max_Brightness);
	}
	else
	{
		printf("FlyCapture: unable to read min max Brightness\n==>Control Unavailable\n");
		min_Brightness = max_Brightness = -1; 
	}

	error = flycaptureGetCameraPropertyRangeEx(	context, 
		FLYCAPTURE_SHUTTER, 
		NULL, NULL, NULL, NULL, NULL, NULL,
		&min_Shutter,
		&max_Shutter);

	if (error == FLYCAPTURE_OK)
	{
		printf("Shutter range [%d : %d]\n",min_Shutter,max_Shutter);
	}
	else
	{
		printf("FlyCapture: unable to read min max Shutter\n==>Control Unavailable\n");
		min_Shutter = max_Shutter = -1; 
	}

	error = flycaptureGetCameraPropertyRangeEx(	context, 
		FLYCAPTURE_GAIN, 
		NULL, NULL, NULL, NULL, NULL, NULL,
		&min_Gain,
		&max_Gain);

	if (error == FLYCAPTURE_OK)
	{
		printf("Gain range [%d : %d]\n",min_Gain,max_Gain);
	}
	else
	{
		printf("FlyCapture: unable to read min max Gain\n==>Control Unavailable\n");
		min_Gain = max_Gain = -1; 
	}

	error = flycaptureGetCameraPropertyRangeEx(	context, 
		FLYCAPTURE_WHITE_BALANCE, 
		NULL, NULL, NULL, NULL, NULL, NULL,
		&min_RBbalance,
		&max_RBbalance);

	if (error == FLYCAPTURE_OK) 
	{
		printf("Red-Blue balance range [%d : %d]\n",min_RBbalance,max_RBbalance);
	}
	else
	{
		printf("FlyCapture: unable to read min max Red-Blue balance\n==>Control Unavailable\n");
		min_RBbalance = max_RBbalance = -1; 
	}

	if(params._brightness >= 0.0 && params._brightness <= 1.0) {
		_setBrightness(params._brightness);
		printf("Brightness %f\n", params._brightness); } 

	if( (params._whiteR >= 0.0) && (params._whiteR <= 1.0) && (params._whiteB >= 0.0) && (params._whiteB <= 1.0)) {
		_setWhiteBalance(params._whiteB, params._whiteR); 
		printf("White balance %f %f\n", params._whiteR, params._whiteB); }

	if(params._shutter >= 0.0 && params._shutter <= 1.0) {
		_setShutter(params._shutter);	// x * 0.0625 = 20 mSec = 50 Hz
		printf("Shutter %f\n", params._shutter); }

	if(params._gain >= 0.0 && params._gain <= 1.0) {
		_setGain(params._gain);	 	// x * -0.0224 = -11.2dB
		printf("Gain %f\n", params._gain); }

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
	if (_acqStarted) {
		error = flycaptureStop(context);
		if (error != FLYCAPTURE_OK)
			return false;
		_acqStarted = false;

		// Destroy the context only if properly started
		// otherwise this might disturb another instance already running
		if (_validContext) {
			error = flycaptureDestroyContext( context );
			if (error != FLYCAPTURE_OK)
				return false;
			_validContext = false;
		}
	}

	// Deallocate buffers
	_destroyBuffers();

	return true;
}

inline bool DragonflyResources::_setBrightness (double value)
{
	if (min_Brightness == -1 && max_Brightness == -1) 
		return false;

	m_Brightness=value;

	int iValue = min_Brightness + int(value * double(max_Brightness - min_Brightness));

	if (iValue < min_Brightness) iValue = min_Brightness;
	if (iValue > max_Brightness) iValue = max_Brightness;

	FlyCaptureError error = flycaptureSetCameraProperty(context, FLYCAPTURE_BRIGHTNESS, iValue, 0, false);

	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

inline bool DragonflyResources::_setWhiteBalance (double blueValue, double redValue)
{
	if (min_RBbalance == -1 && max_RBbalance == -1)
		return false;

	m_WhiteB=blueValue;
	m_WhiteR=redValue;

	fprintf(stderr, "Setting White: %f %f\n", redValue, blueValue);

	int iRedValue = min_RBbalance + int(redValue * double(max_RBbalance - min_RBbalance));
	int iBlueValue = min_RBbalance + int(blueValue * double(max_RBbalance - min_RBbalance));

	if (iRedValue < min_RBbalance) iRedValue = min_RBbalance;
	if (iRedValue > max_RBbalance) iRedValue = max_RBbalance;

	if (iBlueValue < min_RBbalance) iBlueValue = min_RBbalance;
	if (iBlueValue > max_RBbalance) iBlueValue = max_RBbalance;

	FlyCaptureError error = flycaptureSetCameraProperty(context, FLYCAPTURE_WHITE_BALANCE, iRedValue, iBlueValue, false);

	if (error == FLYCAPTURE_OK)
	{
		fprintf(stderr, "setWhiteBalance returned ok\n");
		return true;
	}
	else 
	{
		fprintf(stderr, "setWhiteBalance returned error (are %d %d valid values?)\n", iRedValue, iBlueValue);
		return false;
	}
}

inline bool DragonflyResources::_setShutter (double value)
{
	if (min_Shutter == -1 && max_Shutter == -1) 
		return false;

	m_Shutter=value;

	int iValue = min_Shutter + int(value * double(max_Shutter - min_Shutter));

	if (iValue < min_Shutter) iValue = min_Shutter;
	if (iValue > max_Shutter) iValue = max_Shutter;

	FlyCaptureError error = flycaptureSetCameraProperty(context, FLYCAPTURE_SHUTTER, iValue, 0, false);

	if (error == FLYCAPTURE_OK)
	{
		fprintf(stderr, "setShutter returned ok\n");
		return true;
	}
	else 
	{
		fprintf(stderr, "setShutter returned error (is %d a valid value?)\n", iValue);
		return false;
	}
}

inline bool DragonflyResources::_setGain (double value)
{
	if (min_Gain == -1 && max_Gain == -1) 
		return false;

	m_Gain=value;

	int iValue = min_Gain + int(value * double(max_Gain - min_Gain));

	if (iValue < min_Gain) iValue = min_Gain;
	if (iValue > max_Gain) iValue = max_Gain;

	FlyCaptureError error = flycaptureSetCameraProperty(context, FLYCAPTURE_GAIN, iValue, 0, false);

	if (error == FLYCAPTURE_OK)
	{
		fprintf(stderr, "setGain returned ok\n");
		return true;
	}
	else 
	{
		fprintf(stderr, "setGain returned error (is %d a valid value?)\n", iValue);
		return false;
	}
}

bool DragonflyResources::_setAutoBrightness(bool bAuto)
{
	if (bAuto)
		return flycaptureSetCameraProperty(context, FLYCAPTURE_BRIGHTNESS, 0, 0, true)==FLYCAPTURE_OK;
	else
		return _setBrightness(m_Brightness);
}

bool DragonflyResources::_setAutoShutter(bool bAuto)
{
	if (bAuto)
		return flycaptureSetCameraProperty(context, FLYCAPTURE_SHUTTER, 0, 0, true)==FLYCAPTURE_OK;
	else
		return _setShutter(m_Shutter);
}

bool DragonflyResources::_setAutoGain(bool bAuto)
{
	if (bAuto)
		return flycaptureSetCameraProperty(context, FLYCAPTURE_GAIN, 0, 0, true)==FLYCAPTURE_OK;
	else
		return _setGain(m_Gain);
}

bool DragonflyResources::_setAutoWhiteBalance(bool bAuto)
{
	if (bAuto)
		return flycaptureSetCameraProperty(context, FLYCAPTURE_WHITE_BALANCE, 0, 0, true)==FLYCAPTURE_OK;
	else
		return _setWhiteBalance(m_WhiteB,m_WhiteR);
}

inline double DragonflyResources::_getShutter() const
{
	if (min_Shutter == max_Shutter) 
		return min_Shutter;

	long tmpA;

	FlyCaptureError error = flycaptureGetCameraProperty(context, FLYCAPTURE_SHUTTER, &tmpA, NULL, NULL);
	
	if (error == FLYCAPTURE_OK)
		return double(tmpA - min_Shutter)/double(max_Shutter - min_Shutter);
	else 
		return -1.0;
}

inline double DragonflyResources::_getBrightness() const
{
	if (min_Brightness == max_Brightness)
		return min_Brightness;

	long tmpA;

	FlyCaptureError error = flycaptureGetCameraProperty(context, FLYCAPTURE_BRIGHTNESS, &tmpA, NULL, NULL);

	if (error == FLYCAPTURE_OK)
		return double(tmpA - min_Brightness)/double(max_Brightness - min_Brightness);
	else 
		return -1.0;
}

inline double DragonflyResources::_getGain() const
{
	if (min_Gain == max_Gain)
		return min_Gain;

	long tmpA;

	FlyCaptureError error = flycaptureGetCameraProperty(context, FLYCAPTURE_GAIN, &tmpA, NULL, NULL);
	
	if (error == FLYCAPTURE_OK)
		return double(tmpA - min_Gain)/double(max_Gain - min_Gain);
	else 
		return -1.0;
}

inline bool DragonflyResources::_getWhiteBalance (double &blueValue, double &redValue) const
{
	long tmpRed,tmpBlue;

	FlyCaptureError error = flycaptureGetCameraProperty(context, FLYCAPTURE_WHITE_BALANCE, &tmpRed, &tmpBlue, NULL);

	redValue = double(tmpRed - min_RBbalance)/double(max_RBbalance-min_RBbalance);
	blueValue = double(tmpBlue - min_RBbalance)/double(max_RBbalance-min_RBbalance);

	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
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

	// hmm, we should make sure image has some space in it first, right?
	image.resize(d.sizeX,d.sizeY);

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
	return d._setBrightness(value);
}

bool DragonflyDeviceDriver::setShutter(double value)
{
	DragonflyResources& d = RES(system_resources);
	return d._setShutter(value);
}

bool DragonflyDeviceDriver::setGain(double value)
{
	DragonflyResources& d = RES(system_resources);
	return d._setGain(value);
}

bool DragonflyDeviceDriver::setWhiteBalance(double red, double blue)
{
	DragonflyResources& d = RES(system_resources);
	return d._setWhiteBalance(blue, red);
}

double DragonflyDeviceDriver::getShutter() const
{
	DragonflyResources& d = RES(system_resources);
	return d._getShutter();
}

double DragonflyDeviceDriver::getBrightness() const
{
	DragonflyResources& d = RES(system_resources);
	return d._getBrightness();
}

double DragonflyDeviceDriver::getGain() const
{
	DragonflyResources& d = RES(system_resources);
	return d._getGain();
}

bool DragonflyDeviceDriver::getWhiteBalance(double &red, double &blue) const
{
	DragonflyResources& d = RES(system_resources);
	return d._getWhiteBalance(blue, red);
}

bool DragonflyDeviceDriver::setAutoBrightness(bool bAuto)
{
	DragonflyResources& d = RES(system_resources);
	return d._setAutoBrightness(bAuto);
}

bool DragonflyDeviceDriver::setAutoGain(bool bAuto)
{
	DragonflyResources& d = RES(system_resources);
	return d._setAutoGain(bAuto);
}

bool DragonflyDeviceDriver::setAutoShutter(bool bAuto)
{
	DragonflyResources& d = RES(system_resources);
	return d._setAutoShutter(bAuto);
}

bool DragonflyDeviceDriver::setAutoWhiteBalance(bool bAuto)
{
	DragonflyResources& d = RES(system_resources);
	return d._setAutoWhiteBalance(bAuto);
}

bool DragonflyDeviceDriver::setAuto(bool bAuto)
{
	DragonflyResources& d = RES(system_resources);

	bool bOk=true;

	bOk = bOk && d._setAutoBrightness(bAuto);
	bOk = bOk && d._setAutoGain(bAuto);
	bOk = bOk && d._setAutoShutter(bAuto);
	bOk = bOk && d._setAutoWhiteBalance(bAuto);

	return bOk;
}

void DragonflyDeviceDriver::PrintSettings(){}

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
	int tmpB=0;
	int tmpG=0;
	int tmpR=0;

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
	int tmpB=0;
	int tmpG=0;
	int tmpR=0;

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
	int tmpB=0;
	int tmpG=0;
	int tmpR=0;

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


// converts bayer pattern to BGR
// blue - indicates if the second line contains blue (-1 = YES, 1 = NO)
// starts_with_green - indicates if the line to process has green in the second column (BOOLEAN)
// rgb - indicates if the output color format is RGB ( rgb = 1 ) or BGR (rgb = -1)
// properly setting "blue" and "start_with_green" covers all bayer pattern possibilities
bool Bayer2RGB( const unsigned char* bayer0, int bayer_step, unsigned char *dst0, int dst_step, int width, int height, int blue, int start_with_green, int rgb )
{
	// normalize arguments
	blue = (blue > 0 ? 1 : -1);
	start_with_green = (start_with_green > 0 ? 1 : 0);
	rgb = (rgb > 0 ? 1 : -1);

	memset( dst0, 0, width*3*sizeof(dst0[0]) );
	memset( dst0 + (height - 1)*dst_step, 0, width*3*sizeof(dst0[0]) );
	dst0 += dst_step + 3 + 1;  //jumps to second line, second column, green field
	height -= 2;
	width -= 2;

	for( ; height-- > 0; bayer0 += bayer_step, dst0 += dst_step )
	{
		int t0, t1;
		const unsigned char* bayer = bayer0;
		unsigned char* dst = dst0;
		const unsigned char* bayer_end = bayer + width;

		dst[-4] = dst[-3] = dst[-2] = dst[width*3-1] =
			dst[width*3] = dst[width*3+1] = 0;

		if( width <= 0 )
			continue;

		if( start_with_green )
		{
			t0 = (bayer[1] + bayer[bayer_step*2+1] + 1) >> 1;
			t1 = (bayer[bayer_step] + bayer[bayer_step+2] + 1) >> 1;
			dst[blue*rgb] = (unsigned char)t0;
			dst[0] = bayer[bayer_step+1];
			dst[-blue*rgb] = (unsigned char)t1;
			bayer++;
			dst += 3;
		}

		if( blue > 0 )
		{
			for( ; bayer <= bayer_end - 2; bayer += 2, dst += 6 )
			{
				t0 = (bayer[0] + bayer[2] + bayer[bayer_step*2] +
					bayer[bayer_step*2+2] + 2) >> 2;
				t1 = (bayer[1] + bayer[bayer_step] +
					bayer[bayer_step+2] + bayer[bayer_step*2+1]+2) >> 2;
				dst[rgb] = (unsigned char)t0;
				dst[0] = (unsigned char)t1;
				dst[-rgb] = bayer[bayer_step+1];

				t0 = (bayer[2] + bayer[bayer_step*2+2] + 1) >> 1;
				t1 = (bayer[bayer_step+1] + bayer[bayer_step+3] + 1) >> 1;
				dst[3+rgb] = (unsigned char)t0;
				dst[3] = bayer[bayer_step+2];
				dst[3-rgb] = (unsigned char)t1;
			}
		}
		else
		{
			for( ; bayer <= bayer_end - 2; bayer += 2, dst += 6 )
			{
				t0 = (bayer[0] + bayer[2] + bayer[bayer_step*2] +
					bayer[bayer_step*2+2] + 2) >> 2;
				t1 = (bayer[1] + bayer[bayer_step] +
					bayer[bayer_step+2] + bayer[bayer_step*2+1]+2) >> 2;
				dst[-rgb] = (unsigned char)t0;
				dst[0] = (unsigned char)t1;
				dst[rgb] = bayer[bayer_step+1];

				t0 = (bayer[2] + bayer[bayer_step*2+2] + 1) >> 1;
				t1 = (bayer[bayer_step+1] + bayer[bayer_step+3] + 1) >> 1;
				dst[3-rgb] = (unsigned char)t0;
				dst[3] = bayer[bayer_step+2];
				dst[3+rgb] = (unsigned char)t1;
			}
		}

		if( bayer < bayer_end )
		{
			t0 = (bayer[0] + bayer[2] + bayer[bayer_step*2] +
				bayer[bayer_step*2+2] + 2) >> 2;
			t1 = (bayer[1] + bayer[bayer_step] +
				bayer[bayer_step+2] + bayer[bayer_step*2+1]+2) >> 2;
			dst[blue*rgb] = (unsigned char)t0;
			dst[0] = (unsigned char)t1;
			dst[-blue*rgb] = bayer[bayer_step+1];
			bayer++;
			dst += 3;
		}

		blue = -blue;
		start_with_green = !start_with_green;
	}
	return true;
}


