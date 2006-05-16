
/************************************************************************
//
// Picolo32.h		Version 5.8		Feb. 2001
//
************************************************************************/

#ifndef _PICOLO_
#define _PICOLO_

/****************************************************************************/
/* PICOLO TYPES																*/
#include "E_type.h"

/****************************************************************************/
/* PICOLO BOARD TYPE                                                       */
#define PICOLO_BOARD_UNKNOWN		0x0
#define PICOLO_BOARD_PICOLO			0x1
#define PICOLO_BOARD_PICOLO_PRO		0x2
#define PICOLO_BOARD_PICOLO_PRO2	0x4


/****************************************************************************/
/* PICOLO STATUS CODE                                                       */
#define PICOLO_ACQUISITION_IN_PROGRESS		 2 
#define PICOLO_NO_EVENT						 1
#define PICOLO_OK							 0
#define PICOLO_ERROR_INVALID_HANDLE			-1
#define PICOLO_ERROR_IN_USE					-2
#define PICOLO_ERROR_DEVICE_NOT_FOUND		-3
#define PICOLO_ERROR_HARDWARE_FAILURE		-4
#define PICOLO_ERROR_CANNOT_SET_BUFFER		-5
#define PICOLO_ERROR_TIMEOUT				-6
#define PICOLO_ERROR_NO_MORE_BUFFERS		-7
#define PICOLO_ERROR_NO_TRANSFER_BUFFER		-8
#define PICOLO_ERROR_INVALID_PARAMETER		-9
#define PICOLO_ERROR_NOT_IMPLEMENTED		-10
#define PICOLO_ERROR_COMMUNICATION_FAILED	-11
#define PICOLO_ERROR_NO_SIGNAL				-12


/***************************************************************************/
/* PICOLO FLAGS                                                            */

// -------------------------------------------------------------------------------
// command
#define PICOLO_ACQUIRE_WAIT			0x00000001L // acquire & wait end of acq.
#define PICOLO_ACQUIRE_CONTINUOUS	0x00000002L // acquire without stopping, in the selected buffer
#define PICOLO_ACQUIRE_STOP			0x00000004L // stop any running acquisition
#define PICOLO_ACQUIRE				0x00000008L // acquire

// -------------------------------------------------------------------------------
// additionnal settings
#define PICOLO_ACQUIRE_ARM				0x00100000L // start acquisition on an I/O request
#define PICOLO_ACQUIRE_STROBE			0x00200000L // not yet implemented
#define PICOLO_ACQUIRE_INC				0x00400000L // after action select next buffer in bufferlist
#define PICOLO_ACQUIRE_TRIGGERSTOP		0x00800000L // on trigger, stop acquisition after current field
#define PICOLO_ACQUIRE_SOURCE_SWITCH	0x10000000L 

/***************************************************************************/
/* PICOLO EVENTS                                                           */
// Events
#define PICOLO_EV_END_ACQUISITION	0
#define PICOLO_EV_IO_INTERRUPT		1
#define PICOLO_EV_I2CDONE			3
#define PICOLO_EV_STATUS			4
#define PICOLO_EV_FIFO_OVERRUN		5
#define PICOLO_EV_COUNTER			8
#define	PICOLO_EV_ACQUISITION_ERROR	10


/***************************************************************************/
/* PICOLO BUFFERS                                                          */
#define PICOLO_FIELD_DOWN_ONLY	0x1
#define PICOLO_FIELD_UP_ONLY	0x2 
#define PICOLO_FIELD_ANY		0x10
#define PICOLO_FRAME_DOWN_UP	0x4 
#define PICOLO_FRAME_UP_DOWN	0x8 
#define PICOLO_FRAME_ANY		0x20


/****************************************************************************/
/* PICOLO ColorFormats & Input formats & video sources                      */
#ifndef __COLOR_FORMATS
#define __COLOR_FORMATS
#define PICOLO_COLOR_RGB32		0x1
#define PICOLO_COLOR_RGB24		0x2
#define PICOLO_COLOR_RGB16		0x3
#define PICOLO_COLOR_RGB15		0x4
#define PICOLO_COLOR_YCrCb422	0x5
#define PICOLO_COLOR_YUY2		0x5
#define PICOLO_COLOR_YCrCb411	0x6
#define PICOLO_COLOR_BtYUV		0x6
#define PICOLO_COLOR_Y8			0x7
#define PICOLO_COLOR_GreyScale	0x7
#define PICOLO_COLOR_PL_422		0x9
#define PICOLO_COLOR_PL_411		0xA
#define PICOLO_COLOR_YUV9		0xB
#define PICOLO_COLOR_YUV12		0xC
#define PICOLO_COLOR_RAW		0xD
#define NUMBER_OF_FORMATS		0xF
#endif

// ---------------------------------------------------
// Video Inputs (Picolo)
#define PICOLO_INPUT_SVIDEO_MINIDIN4            0x1 
#define PICOLO_INPUT_SVIDEO_DB9                 0x5 
#define PICOLO_INPUT_COMPOSITE_BNC              0x2 
#define PICOLO_INPUT_COMPOSITE_VIDEO2_DB9       0x3 
#define PICOLO_INPUT_COMPOSITE_VIDEO3_DB9       0x4 
#define PICOLO_INPUT_COMPOSITE_LUMA_MINIDIN4	0x7	

// ---------------------------------------------------
// Video Inputs (Picolo Pro)
#define PICOLO_INPUT_SVIDEO_MINIDIN4            0x1 
#define PICOLO_INPUT_SVIDEO_DB9                 0x5 
#define PICOLO_INPUT_COMPOSITE_BNC              0x2 
#define PICOLO_INPUT_COMPOSITE_DB_VID1			0x40 
#define PICOLO_INPUT_COMPOSITE_DB_VID2			0x50 
#define PICOLO_INPUT_COMPOSITE_DIN_Y			0x60	

// ---------------------------------------------------
// Video Inputs (Picolo Pro 2)
#define PICOLO_INPUT_COMPOSITE_VID1             0x2 
#define PICOLO_INPUT_COMPOSITE_VID2             0x40 
#define PICOLO_INPUT_COMPOSITE_VID3             0x60 
#define PICOLO_INPUT_COMPOSITE_VID4             0x50	

// ---------------------------------------------------
// Video Formats 
#define PICOLO_IFORM_STD_PAL			0x1
#define PICOLO_IFORM_STD_NTSC			0x2
#define PICOLO_IFORM_STD_SECAM			0x3
#define PICOLO_IFORM_PAL_M				0x4
#define PICOLO_IFORM_PAL_N				0x5
#define PICOLO_IFORM_AUTODETECTSVIDEO	0x6
#define PICOLO_IFORM_525LINES			0x7
#define PICOLO_IFORM_625LINES			0x8


/***************************************************************************/
/* PICOLO STATUS                                                           */
#define PICOLO_STATUS_VIDEO_PRESENT		0x01 
#define PICOLO_STATUS_HLOCK				0x02 
#define PICOLO_STATUS_LUMA_OVERFLOW		0x04 
#define PICOLO_STATUS_CHROMA_OVERFLOW	0x08 
#define	PICOLO_STATUS_FIFO_OVERRUN		0x10
#define	PICOLO_STATUS_TIMING_ERROR		0x20


/***************************************************************************/
/* PICOLO FILTER                                                           */
#define PICOLO_FILTER_LUMA_NOTCH				0x1 
#define PICOLO_NO_FILTER_LUMA_NOTCH				0x4 
#define	PICOLO_FULL_RANGE_ADC					0x2
#define	PICOLO_NORMAL_RANGE_ADC					0x8
#define PICOLO_NO_CORING						0x10
#define PICOLO_CORING_8 						0x20
#define PICOLO_CORING_16 						0x40
#define PICOLO_CORING_32 						0x80
#define PICOLO_FILTER_LUMA_COMB					0x100
#define PICOLO_NO_FILTER_LUMA_COMB				0x200
#define PICOLO_FILTER_CHROMA_COMB				0x400
#define PICOLO_NO_FILTER_CHROMA_COMB			0x800
#define PICOLO_ENABLE_GAMMA_CORRECTION_REMOVAL	0x10000
#define PICOLO_DISABLE_GAMMA_CORRECTION_REMOVAL	0x20000 
#define PICOLO_FIELD_DETECTOR_NOISE_FILTER		0x40000	/* Picolo Pro only */
#define PICOLO_NO_FIELD_DETECTOR_NOISE_FILTER	0x80000 /* Picolo Pro only */
#define PICOLO_FILTER_ENABLE_CAGC				0x100000
#define PICOLO_FILTER_DISABLE_CAGC				0x200000

/****************************************************************************/
/*	MISC																	*/
#define PICOLO_BUFFER_ALIGNMENT		64
#define PICOLO_MAX_BUFFER_SIZE		4*768*576 + PICOLO_BUFFER_ALIGNMENT

// COMMUNICATION TIMEOUT
#define PICOLO_STD_TIMEOUT 1000	// 1 sec

// MISCELANEOUS CONSTANTS
#define PICOLO_MAX_EVENTS	32
#define PICOLO_MAX_BUFFERS 1024

#ifndef NULL
#define NULL 0
#endif

#ifndef __FILE_FORMATS
#define __FILE_FORMATS

enum FILE_FORMATS
{ 
  // Tag Image File Format (Adobe)
  FILE_FORMAT_GRAY_LEVEL_TIFF= 1,               // Gray level (8 bits) - PICOLO_COLOR_Y8
  FILE_FORMAT_COLOR_TIFF= 2,                    // Color (3 x 8 bits) - PICOLO_COLOR_RGB24

  // Windows Bitmap (Microsoft)
  FILE_FORMAT_GRAY_LEVEL_BMP= 7,                // Gray level (8 bits) - PICOLO_COLOR_Y8
  FILE_FORMAT_COLOR_BMP =8,                     // Color (3 x 8 bits) - PICOLO_COLOR_RGB24

  // JPEG Interchange File Format
  FILE_FORMAT_GRAY_LEVEL_JPEG= 10,              // Gray level (8 bits) - PICOLO_COLOR_Y8 - See PicoloSetJpegQuality
  FILE_FORMAT_COLOR_JPEG= 11,                   // Color (3 x 8 bits) - PICOLO_COLOR_RGB24 - See PicoloSetJpegQuality
};

// Row pitch constants
#define E_LONGWORD_ALIGNED_ROWS ~0
#define E_CONTIGUOUS_ROWS ~1

#endif // __FILE_FORMATS


/****************************************************************************/
/* PICOLO I/O flags					                                        */
#define	PICOLO_IO1	(1<<0)
#define	PICOLO_IO2	(1<<1)
#define	PICOLO_IO3	(1<<2)
#define	PICOLO_IO4	(1<<3)
#define	PICOLO_IO5	(1<<11)
#define	PICOLO_IO6	(1<<12)
#define	PICOLO_IO7	(1<<13)
#define	PICOLO_IO8	(1<<14)
#define	PICOLO_IO9	(1<<15)
#define	PICOLO_IO10	(1<<16)
#define	PICOLO_IO11	(1<<17)
#define	PICOLO_IO12	(1<<18)
#define	PICOLO_IO13	(1<<19)

#define PICOLO_INPUTEN1		0x00000010L
#define PICOLO_INPUTEN2		0x00000020L
#define PICOLO_INPUTEN3		0x00000040L
#define PICOLO_INPUTEN4		0x00000080L
#define PICOLO_INPUTEN5		0x00000001L
#define PICOLO_INPUTEN6		0x00000002L
#define PICOLO_INPUTEN7		0x00000004L
#define PICOLO_INPUTEN8		0x00000008L
#define PICOLO_INPUTEN9		0x10000000L
#define PICOLO_INPUTEN10	0x20000000L
#define PICOLO_INPUTEN11	0x40000000L
#define PICOLO_INPUTEN12	0x80000000L
#define PICOLO_INPUTEN13	0x01000000L

#define	PICOLO_OUTPUTEN1	0x00000100L
#define	PICOLO_OUTPUTEN2	0x00000200L
#define	PICOLO_OUTPUTEN3	0x00000400L
#define	PICOLO_OUTPUTEN4	0x00000800L
#define	PICOLO_OUTPUTEN5	0x02000000L
#define	PICOLO_OUTPUTEN6	0x04000000L
#define	PICOLO_OUTPUTEN7	0x08000000L
#define	PICOLO_OUTPUTEN8	0x00001000L
#define	PICOLO_OUTPUTEN9	0x00002000L
#define	PICOLO_OUTPUTEN10	0x00004000L
#define	PICOLO_OUTPUTEN11	0x00008000L
#define	PICOLO_OUTPUTEN12	0x00100000L
#define	PICOLO_OUTPUTEN13	0x00200000L

#define	PICOLO_INT_RISING_EDGE		0x00010000L
#define	PICOLO_INT_FALLING_EDGE		0x00020000L
#define	PICOLO_INT_HIGH_LEVEL		0x00040000L
#define	PICOLO_INT_LOW_LEVEL		0x00080000L
#define PICOLO_INT_DISABLE			0x00400000L


#ifndef _PICOLO_CID_
#define _PICOLO_CID_

enum t_PICOLO_CID
{
	// Input Selection + Video Format
	PICOLO_CID_VIDEO_INPUT = 0 ,
	PICOLO_CID_VIDEO_FORMAT = 1 ,

	// Reference adjust.
	PICOLO_CID_REF_SIZEX = 2,
	PICOLO_CID_REF_SIZEY = 3,
	PICOLO_CID_REF_OFFSETX = 4,
	PICOLO_CID_REF_OFFSETY = 5,
	PICOLO_CID_REF_SCALEX = 6,
	PICOLO_CID_REF_SCALEY = 7,

	// Signal operations
	PICOLO_CID_SIGNAL_GAIN = 8,
	PICOLO_CID_SIGNAL_OFFSET = 9,
	PICOLO_CID_SIGNAL_GAIN_U = 10,
	PICOLO_CID_SIGNAL_GAIN_V = 11,
	PICOLO_CID_SIGNAL_HUE = 12,

	// Adjustement
	PICOLO_CID_ADJUST_SIZEX = 13,
	PICOLO_CID_ADJUST_SIZEY = 14,
	PICOLO_CID_ADJUST_OFFSETX = 15,
	PICOLO_CID_ADJUST_OFFSETY = 16,
	PICOLO_CID_ADJUST_SCALEX = 17,
	PICOLO_CID_ADJUST_SCALEY = 18,

	PICOLO_CID_DECIMATION_SETTINGS = 19,
	PICOLO_CID_DECIMATION_VALUE = 20,

	PICOLO_CID_SWITCH_CURRENT_SOURCE = 21,
};

#endif

typedef struct t_CroppingStructure {
	UINT32 UpHDelay;
	UINT32 DwHDelay;
	UINT32 UpHActive;
	UINT32 DwHActive;
	UINT32 UpVDelay;
	UINT32 DwVDelay;
	UINT32 UpVActive;
	UINT32 DwVActive;
} PICOLO_CROPPING_STRUCT ;


/***************************************************************************/
/* ENVIRONMENT DECLARATION                                                 */
#ifdef BUILD_PICOLO_USERMODE_DRIVER
	#ifdef __BORLANDC__
		#ifdef _Windows
			#define DllEntry _export WINAPI
		#else
			#define DllEntry _loadds _saveregs
			#define WINAPI
		#endif
	#elif defined(__WATCOMC__) 
		#define WINAPI
		#define DllEntry
	#else
		#ifdef _WINDOWS
			#define DllEntry __declspec(dllexport) WINAPI
		#else
			#define DllEntry
			#define WINAPI
		#endif
	#endif
#else
	#ifdef __BORLANDC__
		#ifdef _Windows
			#define DllEntry _import WINAPI
		#else
			#define DllEntry
			#define WINAPI
		#endif
	#elif defined(__WATCOMC__) 
		#define WINAPI
		#define DllEntry
	#else
		#ifdef _WINDOWS
			#define DllEntry __declspec(dllimport) WINAPI
		#else
			#define DllEntry
			#define WINAPI
		#endif	
	#endif
#endif 		



#ifdef __cplusplus
extern "C" {
#endif		/* __cplusplus */

/***************************************************************************/
/* PICOLO DATA TYPES                                                       */
typedef long PICOLOHANDLE;
typedef long PICOLOSTATUS;

typedef void (WINAPI *PICOLOEVENTHANDLER)(PVOID context);

PICOLOHANDLE DllEntry PicoloStart	(int index);
PICOLOSTATUS DllEntry PicoloStop	(PICOLOHANDLE hPicolo);
PICOLOSTATUS DllEntry PicoloGetVersion	(PICOLOHANDLE hPicolo, PUINT32 serialNumber, PUINT32 dllVersion, PUINT32 sysVersion );
PICOLOSTATUS DllEntry PicoloGetVersionEx	(PICOLOHANDLE hPicolo, PUINT32 serialNumber, PUINT32 dllVersion, PUINT32 sysVersion, PUINT32 BoardType );

PICOLOSTATUS DllEntry PicoloSelectVideoInput	(PICOLOHANDLE hPicolo, UINT32 videoInput ,UINT32 inputFormat );
PICOLOSTATUS DllEntry PicoloGetVideoInput		(PICOLOHANDLE hPicolo, PUINT32 pVideoInput ,PUINT32 pInputFormat );
PICOLOSTATUS DllEntry PicoloSelectImageFormat	(PICOLOHANDLE hPicolo, UINT32 ImageFormat );
PICOLOSTATUS DllEntry PicoloGetImageFormat		(PICOLOHANDLE hPicolo, PUINT32 pImageFormat );
PICOLOSTATUS DllEntry PicoloGetImageSize		(PICOLOHANDLE hPicolo, PUINT32 Width ,PUINT32 Height );

PICOLOHANDLE DllEntry PicoloSetImageBuffer		(PICOLOHANDLE hPicolo, PVOID buffer, UINT32 bufLen, UINT32 flags, PVOID *alignedBuffer);
PICOLOSTATUS DllEntry PicoloSetBufferList		(PICOLOHANDLE hPicolo, PICOLOHANDLE *bufList, UINT32 count);
PICOLOSTATUS DllEntry PicoloSelectBuffer		(PICOLOHANDLE hPicolo, UINT32 buffer);
PICOLOSTATUS DllEntry PicoloGetCurrentBuffer	(PICOLOHANDLE hPicolo, PUINT32 buffer);
PICOLOSTATUS DllEntry PicoloReleaseImageBuffer	(PICOLOHANDLE hPicolo, PICOLOHANDLE hBuffer);
PICOLOSTATUS DllEntry PicoloGetImageBufferSize	(PICOLOHANDLE hPicolo, PUINT32 bufferSize);

PICOLOSTATUS DllEntry PicoloAcquire		(PICOLOHANDLE hPicolo, UINT32 flags, UINT32 count);
PICOLOSTATUS DllEntry PicoloWaitAcquisitionEnd		(PICOLOHANDLE hPicolo);

PICOLOSTATUS DllEntry PicoloRegisterAcquisitionCallback		(PICOLOHANDLE hPicolo, PICOLOEVENTHANDLER handler, PVOID context);
PICOLOSTATUS DllEntry PicoloUnregisterAcquisitionCallback	(PICOLOHANDLE hPicolo);
PICOLOSTATUS DllEntry PicoloRegisterEventCallback			(PICOLOHANDLE hPicolo, UINT32 event, PICOLOEVENTHANDLER handler, PVOID context);
PICOLOSTATUS DllEntry PicoloUnregisterEventCallback			(PICOLOHANDLE hPicolo, UINT32 event);
PICOLOSTATUS DllEntry PicoloRegisterWindowsMessage			(PICOLOHANDLE hPicolo, HANDLE hWnd, UINT32 event);

PICOLOSTATUS DllEntry PicoloWaitEvent			(PICOLOHANDLE hPicolo, UINT32 event);
PICOLOSTATUS DllEntry PicoloSetWaitTimeout		(PICOLOHANDLE hPicolo, UINT32 timeoutMs);

PICOLOSTATUS DllEntry PicoloEnableIO	(PICOLOHANDLE hPicolo, UINT32 Flags);
PICOLOSTATUS DllEntry PicoloWriteIO		(PICOLOHANDLE hPicolo, UINT32 Value);
PICOLOSTATUS DllEntry PicoloReadIO		(PICOLOHANDLE hPicolo, PUINT32 pValue);

PICOLOSTATUS DllEntry PicoloWriteGain	(PICOLOHANDLE hPicolo, FLOAT32 gain);
PICOLOSTATUS DllEntry PicoloReadGain	(PICOLOHANDLE hPicolo, PFLOAT32 gain);
PICOLOSTATUS DllEntry PicoloWriteOffset	(PICOLOHANDLE hPicolo, FLOAT32 offset);
PICOLOSTATUS DllEntry PicoloReadOffset	(PICOLOHANDLE hPicolo, PFLOAT32 offset);
PICOLOSTATUS DllEntry PicoloWriteUGain	(PICOLOHANDLE hPicolo, FLOAT32 UGain);
PICOLOSTATUS DllEntry PicoloReadUGain	(PICOLOHANDLE hPicolo, PFLOAT32 pUGain);
PICOLOSTATUS DllEntry PicoloWriteVGain	(PICOLOHANDLE hPicolo, FLOAT32 VGain);
PICOLOSTATUS DllEntry PicoloReadVGain	(PICOLOHANDLE hPicolo, PFLOAT32 pVGain);
PICOLOSTATUS DllEntry PicoloWriteHue	(PICOLOHANDLE hPicolo, FLOAT32 Hue);
PICOLOSTATUS DllEntry PicoloReadHue		(PICOLOHANDLE hPicolo, PFLOAT32 Hue);
PICOLOSTATUS DllEntry PicoloWriteHueEx	(PICOLOHANDLE hPicolo, FLOAT32 Hue);
PICOLOSTATUS DllEntry PicoloReadHueEx	(PICOLOHANDLE hPicolo, PFLOAT32 Hue);

PICOLOSTATUS DllEntry PicoloCheckStatus(PICOLOHANDLE hPicolo, PUINT32 pStatus);
PICOLOSTATUS DllEntry PicoloCheckAcquisitionStatus(PICOLOHANDLE hPicolo);
PICOLOSTATUS DllEntry PicoloSetGammaCorrection (PICOLOHANDLE hPicolo, UINT32 Status);
PICOLOSTATUS DllEntry PicoloGetGammaCorrection (PICOLOHANDLE hPicolo, PUINT32 pStatus);

PICOLOSTATUS DllEntry PicoloGetCropping (PICOLOHANDLE hPicolo, PICOLO_CROPPING_STRUCT* pCroppingStruct);
PICOLOSTATUS DllEntry PicoloSetCropping (PICOLOHANDLE hPicolo, PICOLO_CROPPING_STRUCT* pCroppingStruct);

PICOLOSTATUS DllEntry PicoloSourceSwitch( PICOLOHANDLE hPicolo, UINT32 videoInput, UINT32 DelayCount );

PICOLOSTATUS DllEntry PicoloTriggerEvent(PICOLOHANDLE hPicolo, UINT32 event);

PICOLOSTATUS DllEntry PicoloSetFilter (PICOLOHANDLE hPicolo, UINT32 Filter);
PICOLOSTATUS DllEntry PicoloGetFilter (PICOLOHANDLE hPicolo, PUINT32 pFilter);

PICOLOSTATUS DllEntry PicoloGetControlValue (PICOLOHANDLE hPicolo, UINT32 ControlId, PUINT32 pValue);
PICOLOSTATUS DllEntry PicoloGetControlFloat (PICOLOHANDLE hPicolo, UINT32 ControlId, PFLOAT32 pValue);
PICOLOSTATUS DllEntry PicoloSetControlValue (PICOLOHANDLE hPicolo, UINT32 ControlId, UINT32 Value);
PICOLOSTATUS DllEntry PicoloSetControlFloat (PICOLOHANDLE hPicolo, UINT32 ControlId, FLOAT32 Value);

PICOLOSTATUS DllEntry PicoloSetJpegQuality		(PICOLOHANDLE hPicolo, UINT32 Quality);
PICOLOSTATUS DllEntry PicoloSaveImage	( char* pPathName, unsigned Width, unsigned Height
										, short Format, unsigned char* pBuffer, unsigned RowPitch); 

PICOLOSTATUS DllEntry PicoloLoadImage (
  const char* pszPathName, unsigned * pWidth, unsigned * pHeight, short* pFormat,
  unsigned char* (*ImageBuffer)(unsigned Size), unsigned RowPitch);

int DllEntry PicoloImageToMemory(
  char* pMemoryBuffer, unsigned int unMemorySize, unsigned int * unWritten,
  unsigned Width, unsigned Height, short Format,
  const unsigned char* pImageBuffer, unsigned RowPitch);

int DllEntry PicoloImageFromMemory(
  const char* pMemoryBuffer, unsigned int unMemorySize, unsigned int * unRead,
  unsigned * Width, unsigned * Height, short * Format,
  unsigned char* (*ImageBuffer)(unsigned Size), unsigned RowPitch);

PICOLOSTATUS DllEntry PicoloSetOemKey ( PICOLOHANDLE hPicolo, PVOID key );
PICOLOSTATUS DllEntry PicoloCheckOemKey ( PICOLOHANDLE hPicolo, PVOID key );

#ifdef __cplusplus
};
#endif		/* __cplusplus */

#endif /* _PICOLO_ */

