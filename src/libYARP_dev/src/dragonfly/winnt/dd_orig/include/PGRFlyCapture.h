//=============================================================================
// Copyright © 2004 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with Point Grey Research Inc.
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

//=============================================================================
// 
// PGRFlyCapture.h
//
//   Defines the API to the PGR FlyCapture library.
//
//  We welcome your bug reports, suggestions, and comments: 
//  www.ptgrey.com/support/contact
//
//=============================================================================

//=============================================================================
// $Id: PGRFlyCapture.h,v 1.1 2006-05-16 09:57:57 natta Exp $
//=============================================================================
#ifndef __PGRFLYCAPTURE_H__
#define __PGRFLYCAPTURE_H__


#ifdef PGRFLYCAPTURE_EXPORTS
#define PGRFLYCAPTURE_API __declspec( dllexport )
#else
#define PGRFLYCAPTURE_API __declspec( dllimport )
#endif


#ifdef __cplusplus
extern "C"
{
#endif


//
// Description:
//  The version of the library.
//
#define PGRFLYCAPTURE_VERSION 105315


//
// Description:
//   Context pointer for the PGRFlyCapture library.
//
typedef void* FlyCaptureContext;


//
// Description: 
//  The error codes returned by the functions in this library.
//
typedef enum FlyCaptureError
{
   // Function completed successfully.
   FLYCAPTURE_OK,
   // General failure.
   FLYCAPTURE_FAILED,
   // Invalid argument passed.
   FLYCAPTURE_INVALID_ARGUMENT,
   // Invalid context passed.
   FLYCAPTURE_INVALID_CONTEXT,
   // Function not implemented.
   FLYCAPTURE_NOT_IMPLEMENTED,
   // Device already initialized.
   FLYCAPTURE_ALREADY_INITIALIZED,
   // Grabbing has already been started.
   FLYCAPTURE_ALREADY_STARTED,
   // Callback is not registered
   FLYCAPTURE_CALLBACK_NOT_REGISTERED,
   // Callback is already registered
   FLYCAPTURE_CALLBACK_ALREADY_REGISTERED,
   // Problem controlling camera.
   FLYCAPTURE_CAMERACONTROL_PROBLEM,
   // Failed to open file.
   FLYCAPTURE_COULD_NOT_OPEN_FILE,
   // Failed to open a device handle.
   FLYCAPTURE_COULD_NOT_OPEN_DEVICE_HANDLE,
   // Memory allocation error
   FLYCAPTURE_MEMORY_ALLOC_ERROR,
   // flycaptureGrabImage() not called.
   FLYCAPTURE_NO_IMAGE,
   // Device not initialized.
   FLYCAPTURE_NOT_INITIALIZED,
   // flycaptureStart() not called.
   FLYCAPTURE_NOT_STARTED,
   // Request would exceed maximum bandwidth.
   FLYCAPTURE_MAX_BANDWIDTH_EXCEEDED,      
   // Attached camera is not a PGR camera.
   FLYCAPTURE_NON_PGR_CAMERA,
   // Invalid video mode or framerate passed or retrieved.
   FLYCAPTURE_INVALID_MODE,
   // Unknown error.
   FLYCAPTURE_ERROR_UNKNOWN,
   // Invalid custom size.
   FLYCAPTURE_INVALID_CUSTOM_SIZE,
   // Operation timed out.
   FLYCAPTURE_TIMEOUT,
   // Too many image buffers are locked by the user.
   FLYCAPTURE_TOO_MANY_LOCKED_BUFFERS,
   // There is a version mismatch between one of the interacting modules: 
   // pgrflycapture.dll, pgrflycapturegui.dll, and the camera driver.
   FLYCAPTURE_VERSION_MISMATCH,
   // The camera responded that it is currently busy.
   FLYCAPTURE_DEVICE_BUSY,
   // Function has been deprecated.  Please see documentation.
   FLYCAPTURE_DEPRECATED,

} FlyCaptureError;


//
// Description: 
//  An enumeration of the different camera properties that can be set via the
//  API.
//
// Remarks:
//  A lot of these properties are included only for completeness and future
//  expandability, and will have no effect on a PGR camera.
// 
typedef enum FlyCaptureProperty
{
   // The brightness property of the camera.
   FLYCAPTURE_BRIGHTNESS,
   // The auto exposure property of the camera.
   FLYCAPTURE_AUTO_EXPOSURE,
   // The sharpness property of the camera.
   FLYCAPTURE_SHARPNESS,
   // The hardware white balance property of the camera.
   FLYCAPTURE_WHITE_BALANCE,
   // The hue property of the camera.
   FLYCAPTURE_HUE,
   // The saturation property of the camera.
   FLYCAPTURE_SATURATION,
   // The gamma property of the camera.
   FLYCAPTURE_GAMMA,
   // The iris property of the camera.
   FLYCAPTURE_IRIS,
   // The focus property of the camera.
   FLYCAPTURE_FOCUS,
   // The zoom property of the camera.
   FLYCAPTURE_ZOOM,
   // The pan property of the camera.
   FLYCAPTURE_PAN,
   // The tilt property of the camera.
   FLYCAPTURE_TILT,
   // The shutter property of the camera.
   FLYCAPTURE_SHUTTER,
   // The gain  property of the camera.
   FLYCAPTURE_GAIN,
   // The trigger delay property of the camera.
   FLYCAPTURE_TRIGGER_DELAY,
   // The frame rate property of the camera.
   FLYCAPTURE_FRAME_RATE,
   
   //
   // Software white balance property. Use this to manipulate the 
   // values for software whitebalance.  This is only applicable to cameras 
   // that do not do onboard colour processing.  On these cameras, hardware
   // white balance is disabled.
   //
   FLYCAPTURE_SOFTWARE_WHITEBALANCE,

} FlyCaptureProperty;


//
// Description: 
//  The type used to store the serial number uniquely identifying a FlyCapture
//  camera.
//
typedef unsigned long FlyCaptureCameraSerialNumber;


//
// A message returned from the bus callback mechanism indicating a bus reset.
//
#define FLYCAPTURE_MESSAGE_BUS_RESET      0x02

//
// A message returned from the bus callback mechanism indicating a device has
// arrived on the bus.  ulParam contains the serial number of the device.
//
#define FLYCAPTURE_MESSAGE_DEVICE_ARRIVAL 0x03

//
// A message returned from the bus callback mechanism indicating a device has
// been removed from the bus.  ulParam contains the serial number of the 
// device.
//
#define FLYCAPTURE_MESSAGE_DEVICE_REMOVAL 0x04

//
// Function prototype for the bus callback mechanism.  pparam contains the 
// parameter passed in when registering the callback.  iMessage is one of the
// above FLYCAPTURE_MESSAGE_* #defines and ulParam is a message-defined 
// parameter.
//
typedef void __cdecl 
FlyCaptureCallback( void* pparam, int iMessage, unsigned long ulParam );


//
// A value indicating an infinite wait sent to a wait function.
//
#define FLYCAPTURE_INFINITE   0xFFFFFFFF


//
// Description:
//   Enum describing different framerates.
//
typedef enum FlyCaptureFrameRate
{
   // 1.875 fps. (Frames per second)
   FLYCAPTURE_FRAMERATE_1_875,
   // 3.75 fps.
   FLYCAPTURE_FRAMERATE_3_75,
   // 7.5 fps.
   FLYCAPTURE_FRAMERATE_7_5,
   // 15 fps.
   FLYCAPTURE_FRAMERATE_15,
   // 30 fps.
   FLYCAPTURE_FRAMERATE_30,
   // Deprecated.  Please use Custom image.
   FLYCAPTURE_FRAMERATE_50,
   // 60 fps.
   FLYCAPTURE_FRAMERATE_60,
   // 120 fps.
   FLYCAPTURE_FRAMERATE_120,
   // 240 fps.
   FLYCAPTURE_FRAMERATE_240,
   // Number of possible camera frame rates.
   FLYCAPTURE_NUM_FRAMERATES,
   // Custom frame rate.  Used with custom image size functionality.
   FLYCAPTURE_FRAMERATE_CUSTOM,
   // Hook for "any usable frame rate."
   FLYCAPTURE_FRAMERATE_ANY,

} FlyCaptureFrameRate;


//
// Description:
//   Enum describing different video modes.
//
// Remarks:
//   The explicit numbering is to provide downward compatibility for this enum.
//
typedef enum FlyCaptureVideoMode
{
   // 160x120 YUV444.
   FLYCAPTURE_VIDEOMODE_160x120YUV444     = 0,
   // 320x240 YUV422.
   FLYCAPTURE_VIDEOMODE_320x240YUV422     = 1,
   // 640x480 YUV411.
   FLYCAPTURE_VIDEOMODE_640x480YUV411     = 2,
   // 640x480 YUV422.
   FLYCAPTURE_VIDEOMODE_640x480YUV422     = 3,
   // 640x480 24-bit RGB.
   FLYCAPTURE_VIDEOMODE_640x480RGB        = 4,
   // 640x480 8-bit greyscale or bayer tiled color image.
   FLYCAPTURE_VIDEOMODE_640x480Y8         = 5,
   // 640x480 16-bit greyscale or bayer tiled color image.
   FLYCAPTURE_VIDEOMODE_640x480Y16        = 6,
   // 800x600 YUV422.
   FLYCAPTURE_VIDEOMODE_800x600YUV422     = 17,
   // 800x600 RGB.
   FLYCAPTURE_VIDEOMODE_800x600RGB        = 18,
   // 800x600 8-bit greyscale or bayer tiled color image.
   FLYCAPTURE_VIDEOMODE_800x600Y8         = 7,
   // 800x600 16-bit greyscale or bayer tiled color image.
   FLYCAPTURE_VIDEOMODE_800x600Y16        = 19,
   // 1024x768 YUV422.
   FLYCAPTURE_VIDEOMODE_1024x768YUV422    = 20,
   // 1024x768 RGB.
   FLYCAPTURE_VIDEOMODE_1024x768RGB       = 21,
   // 1024x768 8-bit greyscale or bayer tiled color image.
   FLYCAPTURE_VIDEOMODE_1024x768Y8        = 8,
   // 1024x768 16-bit greyscale or bayer tiled color image.
   FLYCAPTURE_VIDEOMODE_1024x768Y16       = 9,
   // 1280x960 YUV422.
   FLYCAPTURE_VIDEOMODE_1280x960YUV422    = 22,
   // 1280x960 RGB.
   FLYCAPTURE_VIDEOMODE_1280x960RGB       = 23,
   // 1280x960 8-bit greyscale or bayer titled color image.
   FLYCAPTURE_VIDEOMODE_1280x960Y8        = 10,
   // 1280x960 16-bit greyscale or bayer titled color image.
   FLYCAPTURE_VIDEOMODE_1280x960Y16       = 24,
   // 1600x1200 YUV422.
   FLYCAPTURE_VIDEOMODE_1600x1200YUV422   = 50,
   // 1600x1200 RGB.
   FLYCAPTURE_VIDEOMODE_1600x1200RGB      = 51,
   // 1600x1200 8-bit greyscale or bayer titled color image.
   FLYCAPTURE_VIDEOMODE_1600x1200Y8       = 11,
   // 1600x1200 16-bit greyscale or bayer titled color image.
   FLYCAPTURE_VIDEOMODE_1600x1200Y16      = 52,
   
   // Custom video mode.  Used with custom image size functionality.
   FLYCAPTURE_VIDEOMODE_CUSTOM            = 15,
   // Hook for "any usable video mode."
   FLYCAPTURE_VIDEOMODE_ANY               = 16,

   // Number of possible video modes.
   FLYCAPTURE_NUM_VIDEOMODES              = 23,

} FlyCaptureVideoMode;


//
// Description: 
//  An enumeration used to describe the different camera models that can be 
//  accessed through this SDK.
//
typedef enum FlyCaptureCameraModel
{
   FLYCAPTURE_FIREFLY,
   FLYCAPTURE_DRAGONFLY,
   FLYCAPTURE_AIM,
   FLYCAPTURE_SCORPION,
   FLYCAPTURE_TYPHOON,
   FLYCAPTURE_FLEA,
   FLYCAPTURE_DRAGONFLY_EXPRESS,
   FLYCAPTURE_UNKNOWN = -1,

} FlyCaptureCameraModel;


//
// Description: 
//  An enumeration used to describe the different camera color configurations.
//
typedef enum FlyCaptureCameraType
{
   // black and white system.
   FLYCAPTURE_BLACK_AND_WHITE,
   // color system.
   FLYCAPTURE_COLOR

} FlyCaptureCameraType;


//
// Description: 
//  Camera information structure.  This structure will eventually be replaced
//  by FlyCaptureInfoEx.
//
typedef struct FlyCaptureInfo
{
   // camera serial number.
   FlyCaptureCameraSerialNumber	 SerialNumber;
   // type of CCD (color or b&w).
   FlyCaptureCameraType		 CameraType;
   // Camera model.
   FlyCaptureCameraModel         CameraModel;
   // Null-terminated camera model string for attached camera.
   char                          pszModelString[ 512 ];

} FlyCaptureInfo;


//
// Description: 
//  Extended camera information.  This structure will eventually replace
//  FlyCaptureInfo.
//
typedef struct FlyCaptureInfoEx
{
   // camera serial number.
   FlyCaptureCameraSerialNumber	 SerialNumber;
   // type of CCD (color or b&w).
   FlyCaptureCameraType		 CameraType;
   // Camera model.
   FlyCaptureCameraModel         CameraModel;
   // Camera model string.  Null terminated.
   char  pszModelName[ 512 ];
   // Vendor name string.  Null terminated.
   char  pszVendorName[ 512 ];
   // Sensor info string.  Null terminated.
   char  pszSensorInfo[ 512 ];
   // 1394 DCAM compliance level.  DCAM version is this value / 100. eg, 1.31.
   int   iDCAMVer;
   // Low-level 1394 node number for this device.
   int   iNodeNum;
   // Low-level 1394 bus number for this device.
   int   iBusNum;
   // Reserved for future data.
   unsigned long  ulReserved[ 120 ];
   
} FlyCaptureInfoEx;


//
// Description:
//   An enumeration used to describe the different color processing 
//   method.
//
// Remarks:
//   This is only relevant for cameras that do not do onboard color
//   processing, such as the Dragonfly.  The FLYCAPTURE_RIGOROUS
//   method is very slow and will not keep up with high frame rates.
//
typedef enum FlyCaptureColorMethod
{
   // Disable color processing.
   FLYCAPTURE_DISABLE,
   // Edge sensing de-mosaicing.  This is the most accurate method
   // that can still keep up with the camera's frame rate.
   FLYCAPTURE_EDGE_SENSING,
   // Nearest neighbor de-mosaicing.  This algorithm is significantly
   // faster than edge sensing, at the cost of accuracy.
   FLYCAPTURE_NEAREST_NEIGHBOR,
   // Faster, less accurate nearest neighbor de-mosaicing.
   FLYCAPTURE_NEAREST_NEIGHBOR_FAST,
   // Rigorous de-mosaicing.  This provides the best quality colour
   // reproduction.  This method is so processor intensive that it
   // might not keep up with the camera's frame rate.  Best used for
   // offline processing where accurate colour reproduction is required.
   FLYCAPTURE_RIGOROUS,

} FlyCaptureColorMethod;


//
// Description:
//   An enumeration used to indicate the format of the stippled images
//   passed into a destippling function.
//
// Remarks:
//   This is only relevant for cameras that do not do onboard color
//   processing, such as the Dragonfly.  The four letters of the enum
//   value correspond to the "top left" 2x2 section of the stippled image.
//   For example, the first line of a BGGR image image will be
//   BGBGBG..., and the second line will be GRGRGR....
//
typedef enum FlyCaptureStippledFormat
{
   // Indicates a BGGR image.
   FLYCAPTURE_STIPPLEDFORMAT_BGGR,
   // Indicates a GBRG image.
   FLYCAPTURE_STIPPLEDFORMAT_GBRG,
   // Indicates a GRBG image.
   FLYCAPTURE_STIPPLEDFORMAT_GRBG,
   // Indicates a RGGB image.
   FLYCAPTURE_STIPPLEDFORMAT_RGGB,
   // Indicates the default stipple format for the Dragonfly or Firefly.
   FLYCAPTURE_STIPPLEDFORMAT_DEFAULT

} FlyCaptureStippledFormat;


//
// Description:
//   An enumeration used to indicate the type of the returned pixels.  This
//   enumeration is used as a member of FlyCaptureImage and as a parameter
//   to FlyCaptureStartCustomImage.
//
typedef enum FlyCapturePixelFormat
{
   // 8 bits of mono information.
   FLYCAPTURE_MONO8     = 0x00000001,
   // YUV 4:1:1.
   FLYCAPTURE_411YUV8   = 0x00000002,
   // YUV 4:2:2.
   FLYCAPTURE_422YUV8   = 0x00000004,
   // YUV 4:4:4.
   FLYCAPTURE_444YUV8   = 0x00000008,
   // R = G = B = 8 bits.
   FLYCAPTURE_RGB8      = 0x00000010,
   // 16 bits of mono information.
   FLYCAPTURE_MONO16    = 0x00000020,
   // R = G = B = 16 bits.
   FLYCAPTURE_RGB16     = 0x00000040,
   // 16 bits of signed mono information.
   FLYCAPTURE_S_MONO16  = 0x00000080,
   // R = G = B = 16 bits signed.
   FLYCAPTURE_S_RGB16   = 0x00000100,
   // 8 bit raw data output of sensor.
   FLYCAPTURE_RAW8      = 0x00000200,
   // 16 bit raw data output of sensor.
   FLYCAPTURE_RAW16     = 0x00000400,
   // 24 bit BGR
   FLYCAPTURE_BGR       = 0x10000001,
   // 32 bit BGRU
   FLYCAPTURE_BGRU      = 0x10000002,
   // Unused member to force this enum to compile to 32 bits.
   FCPF_FORCE_QUADLET   = 0x7FFFFFFF,

} FlyCapturePixelFormat;


//
// Description:
//   Enumerates the image file formats that flycaptureSaveImage() can write to.
//
typedef enum FlyCaptureImageFileFormat
{
   // Single channel (8 or 16 bit) greyscale portable grey map.
   FLYCAPTURE_FILEFORMAT_PGM,
   // 3 channel RGB portable pixel map.
   FLYCAPTURE_FILEFORMAT_PPM,
   // 3 or 4 channel RGB windows bitmap.
   FLYCAPTURE_FILEFORMAT_BMP,
   // JPEG format.  Not implemented.
   FLYCAPTURE_FILEFORMAT_JPG,
   // Portable Network Graphics format.  Not implemented.
   FLYCAPTURE_FILEFORMAT_PNG,
   // Raw data output.
   FLYCAPTURE_FILEFORMAT_RAW,
      
} FlyCaptureImageFileFormat;


//
// Description:
//  This structure defines the format by which time is represented in the 
//  PGRFlycapture SDK.  The ulSeconds and ulMicroSeconds values represent the
//  absolute system time when the image was captured.  The ulCycleSeconds
//  and ulCycleCount are higher-precision values that have either been 
//  propagated up from the 1394 bus or extracted from the image itself.  The 
//  data will be extracted from the image if image timestamping is enabled and
//  directly (and less accurately) from the 1394 bus otherwise.
//
//  The ulCycleSeconds value will wrap around after 128 seconds.  The ulCycleCount 
//  represents the 1/8000 second component. Use these two values when synchronizing 
//  grabs between two computers sharing a common 1394 bus that may not have 
//  precisely synchronized system timers.
//
typedef struct FlyCaptureTimestamp
{
   // The number of seconds since the epoch. 
   unsigned long ulSeconds;
   // The microseconds component.
   unsigned long ulMicroSeconds;
   // The cycle time seconds.  0-127.
   unsigned long ulCycleSeconds;
   // The cycle time count.  0-7999. (1/8000ths of a second.)
   unsigned long ulCycleCount;
   // The cycle offset.  0-3071 (1/3072ths of a cycle count.)
   unsigned long ulCycleOffset;
   
} FlyCaptureTimestamp;


//
// Description:
//  This structure is used to pass image information into and out of the
//  API.
//
// Remarks:
//  The size of the image buffer is iRowInc * iRows, and depends on the
//  pixel format.
//
typedef struct FlyCaptureImage
{
   // Rows, in pixels, of the image.
   int iRows;
   // Columns, in pixels, of the image.
   int iCols;
   // Row increment.  The number of bytes per row.
   int iRowInc;
   // Video mode that this image was captured with.  Only populated when the
   // image is returned from a grab call.
   FlyCaptureVideoMode videoMode;
   // Timestamp of this image.
   FlyCaptureTimestamp timeStamp;
   // Pointer to the actual image data.
   unsigned char* pData;
   //
   // If the returned image is Y8 or Y16, this flag indicates whether it is
   // a greyscale or stippled (bayer tiled) image.  In modes other than Y8
   // or Y16, this flag has no meaning.
   //
   bool bStippled;
   // The pixel format of this image.
   FlyCapturePixelFormat pixelFormat;
   // Reserved for future use.
   unsigned long  ulReserved[ 6 ];

} FlyCaptureImage;



//=============================================================================
// FlyCapture Bus Functions
//=============================================================================
// Group = FlyCapture Bus

//-----------------------------------------------------------------------------
//
// Name: flycaptureBusCameraCount()
//
// Description:
//   This function returns the number of cameras on the 1394 bus.
//
// Arguments:
//   puiCount - Returns the number of cameras on the bus.
// 
// Returns: 
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureBusCameraCount(
			 unsigned int* puiCount );


//-----------------------------------------------------------------------------
//
// Name: flycaptureBusEnumerateCameras()
//
// Description:
//
//   Deprecated.  Please use flycaptureBusEnumerateCamerasEx().
//
// Returns: 
//   A FlyCaptureError indicating the success or failure of the function.
//
// See Also:
//   flycaptureBusCameraCount()
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureBusEnumerateCameras( 
			      FlyCaptureInfo*   pFlyCaptureInfoArray,
			      unsigned int*     puiSize );


//-----------------------------------------------------------------------------
//
// Name: flycaptureBusEnumerateCamerasEx()
//
// Description:
//   This function enumerates all of the FlyCapture cameras found on the 1394 
//   bus.  It fills an array of FlyCaptureInfoEx structures with all of the 
//   pertinent information from the attached cameras. The index of a given 
//   FlyCaptureInfo structure in the array parInfo is the device number.
//
// Arguments:
//   arInfo  - An array of FlyCaptureInfoEx structures, at least as
//             large as the number of FlyCapture cameras on the bus.
//   puiSize - The size of the array passed in.  The number of cameras
//             detected is passed back in this argument also.
//
// Returns: 
//   A FlyCaptureError indicating the success or failure of the function.
//
// See Also:
//   flycaptureBusCameraCount()
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureBusEnumerateCamerasEx( 
                                FlyCaptureInfoEx*  arInfo,
                                unsigned int*      puiSize );


//-----------------------------------------------------------------------------
//
// Name: flycaptureModifyCallback()
//
// Description:
//   This function registers or deregisters a bus callback function.
//   When the state of the bus changes, the FlyCaptureCallback
//   function will be called with an integer message parameter.  Please see
//   the FlyCap example for more information on how to use callback
//   functionality.
//
// Arguments:
//   context - The FlyCapture context to access.
//   pfnCallback - A pointer to an externally defined callback function.
//   pparam - A user-specified parameter to be passed back to the callback 
//            function.  Can be NULL.
//   bAdd - true if the callback is to be added to the list of callbacks,
//          false if the callback is to be removed.
// 
// Returns: 
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureModifyCallback(
                         FlyCaptureContext   context,
                         FlyCaptureCallback* pfnCallback,
                         void*               pparam,
                         bool                bAdd );


//=============================================================================
// Construction/Destruction
//=============================================================================
// Group = Construction/Destruction

//-----------------------------------------------------------------------------
//
// Name: flycaptureCreateContext()
//
// Description:
//   This function creates a FlyCaptureContext and allocates all of the memory 
//   that it requires.  The purpose of the FlyCaptureContext is to act as a 
//   handle to one of the cameras attached to the system. This call must be 
//   made before any other calls involving the context will work.
//
// Arguments:
//   pContext - A pointer to the FlyCaptureContext to be created.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureCreateContext( 
			FlyCaptureContext* pContext );


//-----------------------------------------------------------------------------
//
// Name: flycaptureDestroyContext() 
//
// Description:
//   Destroys the given FlyCaptureContext.  In order to prevent memory leaks 
//   from occurring, this function must be called when the user is finished 
//   with the FlyCaptureContext. 
//
// Arguments:
//   context - The FlyCaptureContext to be destroyed.
//
// Returns:		  
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureDestroyContext( 
			 FlyCaptureContext context );


//-----------------------------------------------------------------------------
//
// Name: flycaptureInitialize() 
//
// Description:
//   This function initializes one of the cameras on the bus and associates it
//   with the provided FlyCaptureContext. This call must be made after a 
//   flycaptureCreateContext() command and prior to a flycaptureStart() command 
//   in order for images to be grabbed.  Users can also use the 
//   flycaptureInitializeFromSerialNumber() command to initialize a FlyCapture 
//   with a specific serial number.
//
// Arguments:
//   context - The FlyCaptureContext to be associated with the camera being 
//             initialized.
//   ulDevice - The 1394 bus index of the FlyCapture camera to be initialized.
//
// Note:
//  If there is only one FlyCapture on the bus, its index is generally 0.
// 
// Returns: 
//   A FlyCaptureError indicating the success or failure of the function.
//
// See Also:
//   flycaptureInitializeFromSerialNumber(), flycaptureCreateContext(), 
//   flycaptureStart()
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureInitialize(
		     FlyCaptureContext context,
		     unsigned long     ulDevice );


//-----------------------------------------------------------------------------
//
// Name: flycaptureInitializeFromSerialNumber()
//
// Description:
//   Similar to the flycaptureInitialize() command, this function initializes 
//   one of the cameras on the bus and associates it with the given 
//   FlyCaptureContext.  This function differs from its counterpart in that it
//   takes a serial number rather than a bus index.
//
// Arguments:
//   context - The FlyCaptureContext to be associated with the camera being 
//             initialized.
//   serialNumber - The serial number of the FlyCapture camera system to be 
//                  initialized.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// See Also:
//   flycaptureInitialize(), flycaptureCreateContext(), flycaptureStart()
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureInitializeFromSerialNumber(
				     FlyCaptureContext		  context,
				     FlyCaptureCameraSerialNumber serialNumber );


//-----------------------------------------------------------------------------
//
// Name: flycaptureGetCameraInformation()
//
// Description:
//   Deprecated.  Please use flycaptureGetCameraInfo().
//
// Arguments:
//   context - The FlyCaptureContext to access the FlyCapture camera.
//   pFlyCaptureInfo - A pointer to a FlyCaptureInfo structure.
// 
// Returns: 
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureGetCameraInformation(
			       FlyCaptureContext context,
                               FlyCaptureInfo*   pFlyCaptureInfo );


//-----------------------------------------------------------------------------
//
// Name: flycaptureGetCameraInfo()
//
// Description:
//   Retrieves information about the camera.
//
// Arguments:
//   context   - The FlyCaptureContext to access the FlyCapture camera.
//   pInfo     - Receives the camera information.
// 
// Returns: 
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureGetCameraInfo(
                        FlyCaptureContext context,
                        FlyCaptureInfoEx* pInfo );



//=============================================================================
// General Functions
//=============================================================================
// Group = General

//-----------------------------------------------------------------------------
//
// Name: flycaptureGetLibraryVersion()
//
// Description:
//  This function returns the version of the library in the format 
//  100*(major version)+(minor version).
//
// Returns:
//  An integer indicating the current version of the library.
//
PGRFLYCAPTURE_API int
flycaptureGetLibraryVersion();


//-----------------------------------------------------------------------------
//
// Name: FlyCaptureErrorToString() 
//
// Description:
//    This function returns a description of the provided FlyCaptureError.
//
// Arguments:
//   error - The FlyCapture error to be parsed.
//
// Returns: 
//   A null-terminated character string that describes the FlyCapture error.
//
PGRFLYCAPTURE_API const char*
flycaptureErrorToString( 
			FlyCaptureError error );


//=============================================================================
// Control Functions
//=============================================================================
// Group = Control Functions

//-----------------------------------------------------------------------------
//
// Name: flycaptureCheckVideoMode()
//
// Description:
//   This function allows the user to check if a given mode is supported by the
//   camera.
//
// Arguments:
//   context - An initialized FlyCaptureContext.
//   videoMode - The video mode to check.
//   frameRate - The frame rate to check.
//   pbSupported - A pointer to a bool that will store whether or not the mode 
//                 is supported.
// 
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureCheckVideoMode(
			 FlyCaptureContext   context,
			 FlyCaptureVideoMode videoMode,
			 FlyCaptureFrameRate frameRate,
			 bool*		     pbSupported );


//-----------------------------------------------------------------------------
//
// Name: flycaptureGetCurrentVideoMode()
//
// Description:
//   This function allows the user to request the current video mode.
//
// Arguments:
//   context - An initialized FlyCaptureContext.
//   pVideoMode - A pointer to a video mode to be filled in.
//   pFrameRate - A pointer to a frame rate to be filled in.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetCurrentVideoMode(
			      FlyCaptureContext    context,
			      FlyCaptureVideoMode* pVideoMode,
			      FlyCaptureFrameRate* pFrameRate);


//-----------------------------------------------------------------------------
//
// Name: flycaptureGetColorProcessingMethod()
//
// Description:
//   This function allows users to check the current color processing method.
//
// Arguments:
//   context - The FlyCapture context to access.
//   pMethod - A pointer to a FlyCaptureColorMethod that will store the current
//             color processing method.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//  
// See Also:
//  flycaptureSetColorProcessingMethod()
//  
//
// Remarks:
//  This function is only applicable when using the SDK and driver with cameras
//  that do not do on board color processing, such as the dragonfly.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetColorProcessingMethod( 
				   FlyCaptureContext        context,
				   FlyCaptureColorMethod*   pMethod );


//-----------------------------------------------------------------------------
//
// Name: flycaptureSetColorProcessingMethod()
//
// Description:
//   This function allows users to select the method used for color processing.
//
// Arguments:
//   context - The FlyCapture context to access.
//   method  - A variable of type FlyCaptureColorMethod indicating the color
//             processing method to be used.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Note:
//   Selecting FLYCAPTURE_DISABLE turns off all color processing.  
//   FLYCAPTURE_NEAREST_NEIGHBOR uses a fast nearest neighbor approach to 
//   de-mosaic the images and DIGICLOPS_EDGE_SENSING uses a more expensive 
//   method to produce higher quality color images.
//  
// See Also:
//   flycaptureGetColorProcessingMethod()
//
// Remarks:
//  This function is only applicable when using the SDK and driver with cameras
//  that do not do on board color processing, such as the dragonfly.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetColorProcessingMethod( 
				   FlyCaptureContext	    context,
				   FlyCaptureColorMethod    method );


//-----------------------------------------------------------------------------
//
// Name: flycaptureGetColorTileFormat()
//
// Description:
//   This function allows users to check the current color tile destippling
//   format.
//
// Arguments:
//   context - The FlyCapture context to access.
//   pformat - A pointer to a FlyCaptureStippledFormat that will store the current
//             color tile format.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//  
// Remarks:
//  The color tile format indicates the format of the stippled image the camera
//  returns.  This function is only applicable to cameras that do not do
//  onboard color processing.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetColorTileFormat(
			      FlyCaptureContext		 context,
			      FlyCaptureStippledFormat*  pformat );


//-----------------------------------------------------------------------------
//
// Name: flycaptureSetColorTileFormat()
//
// Description:
//   This function sets the color tile destippling format.
//
// Arguments:
//   context - The FlyCapture context to access.
//   format  - The FlyCaptureStippledFormat to set.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//  
// Remarks:
//  The color tile format indicates the format of the stippled image the camera
//  returns.  This function is only applicable to cameras that do not do
//  onboard color processing.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetColorTileFormat(
			     FlyCaptureContext        context,
			     FlyCaptureStippledFormat format );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureStart()
//
// Description:
//   This function starts the image grabbing process.  It should be called 
//   after flycaptureCreateContext() and flycaptureInitialize().
//
// Arguments:
//   context - The FlyCaptureContext to start grabbing.
//   videoMode - The video mode to start the camera in.
//   frameRate - The frame rate to start the camera at.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// See Also:
//   flycaptureCreateContext(), flycaptureInitialize(), 
//   flycaptureInitializeFromSerialNumber(), flycaptureStop()
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureStart(
		FlyCaptureContext   context,
		FlyCaptureVideoMode videoMode, 
		FlyCaptureFrameRate frameRate );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureQueryCustomImage()
//
// Description:
//   This function queries the options available for the advanced Custom Image
//   functionality.
//   
// Arguments:
//   context - The FlyCaptureContext to start grabbing.
//   uiMode - The mode to query (0-7).
//   pbAvailable - Indicates the availability of this mode.
//   puiMaxImagePixelsWidth - Maximum horizonal pixels.
//   puiMaxImagePixelsHeight - Maximum vertical pixels.
//   puiPixelUnitHorz - Indicates the horizonal "step size" of the custom 
//                      image.
//   puiPixelUnitVert - Indicates the vertical "step size" of the custom 
//                      image.
//   puiPixelFormats  - A bit field indicating the supported pixel formats of
//                      this mode.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// See Also:
//   flycaptureStartCustomImage()
//   
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureQueryCustomImage(
			   FlyCaptureContext context,
			   unsigned int	     uiMode,
			   bool*	     pbAvailable,
			   unsigned int*     puiMaxImagePixelsWidth,
			   unsigned int*     puiMaxImagePixelsHeight,
			   unsigned int*     puiPixelUnitHorz,
			   unsigned int*     puiPixelUnitVert,
                           unsigned int*     puiPixelFormats );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureStartCustomImage()
//
// Description:
//   This function starts the image grabbing process with advanced "custom
//   image" (DCAM Format 7) functionality, which allows the user to select a 
//   custom image size and/or region of interest.
//
// Arguments:
//   context         - The FlyCaptureContext to start grabbing.
//   uiMode          - The camera-specific mode.  (0-7).
//   uiImagePosLeft  - The left position of the (sub)image.
//   uiImagePosTop   - Top top position of the (sub)image.
//   uiWidth         - The width of the (sub)image.
//   uiHeight        - The height of the (sub)image.
//   fBandwidth      - A number between 1.0 and 100.0 which represents the 
//                     percentage of the camera's maximum bandwidth to use for
//                     transmission.
//   format          - The pixel format to be used.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// See Also:
//   flycaptureStartCustomImagePacket(), flycaptureQueryCustomImage()
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureStartCustomImage(
			   FlyCaptureContext       context,
			   unsigned int	           uiMode,
			   unsigned int            uiImagePosLeft,
			   unsigned int            uiImagePosTop,
			   unsigned int	           uiWidth,
			   unsigned int	           uiHeight,
			   float                   fBandwidth,
                           FlyCapturePixelFormat   format );


//-----------------------------------------------------------------------------
//
// Name:
//  flycaptureStop() 
//
// Description:
//   This function halts all image grabbing for the specified FlyCaptureContext.
//
// Arguments:
//   context - The FlyCaptureContext to stop.
//
// Returns:       
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   This function invalidates all buffers returned by flycaptureLockNext() 
//   and flycaptureLockLatest().
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureStop( 
	       FlyCaptureContext context );


//=============================================================================
// Image Related Functions
//=============================================================================
// Group = Image Related Functions


//-----------------------------------------------------------------------------
//
// Name:  flycaptureGrabImage() 
//
// Description:
//   This function grabs the newest image from the FlyCapture camera system and
//   passes the image buffer and information to the user.
//
// Arguments:
//   context - The FlyCapture context to lock the image in.
//   ppImageBuffer - Pointer to the returned image buffer pointer.
//   piRows - Pointer to the returned rows.
//   piCols - Pointer to the returned columns.
//   piRowInc - Pointer to the returned row increment (number of bytes per row.)
//   pVideoMode - Pointer to the returned video mode.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//  This function will block until a new image is available.  You can 
//  optionally set the timeout value for the wait using the 
//  flycaptureSetGrabTimeoutEx() function (by default the wait time is 
// infinite.) Setting the timeout value should normally not be necessary.
//
// See Also:
//  flycaptureStart(), flycaptureGrabImage2(), flycaptureSetGrabTimeoutEx()
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGrabImage( 
		    FlyCaptureContext	  context,
		    unsigned char**	  ppImageBuffer,
		    int*		  piRows,
		    int*		  piCols,
		    int*		  piRowInc,
		    FlyCaptureVideoMode*  pVideoMode  );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureGrabImage2()
//
// Description:
//   This function is identical to flycaptureGrabImage() except that it returns
//   a FlyCaptureImage.
//
// Arguments:
//   context - The FlyCapture context to lock the image in.
//   pimage  - A pointer to a FlyCaptureImage structure that will contain the
//             image information.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//  See remarks for flycaptureGrabImage().
//
// See Also:
//  flycaptureStart(), flycaptureGrabImage(), flycaptureSetGrabTimeoutEx()
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGrabImage2( 
		     FlyCaptureContext	  context,
		     FlyCaptureImage*	  pimage   );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureWritePPM()
//
// Description:
//   Deprecated.  Please use flycaptureSaveImage().
//
// Arguments:
//   pImageBuffer    - Pointer to the image buffer to write from.
//   iRows	     - Image rows.
//   iCols	     - Image columns.
//   pszFileName     - The name of the file to write to.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//  The image must be in BGR24 format, ie, the output from any of the
//  conversion or colour processing routines.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureWritePPM(
		   const unsigned char*   pImageBuffer,
		   int                    iRows,
		   int                    iCols,
		   const char*            pszFileName );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureSaveImage()
//
// Description:
//   Writes the specified image buffer to disk.
//
// Arguments:
//   context   - The FlyCapture context to access.
//   pimage    - The image to save.  This can be populated by the user, by only
//               filling out the pData, size, and pixel format information, or
//               can be the structure returned by flycaptureConvertImage().
//   pszPath   - The name of the file to write to.
//   format    - The file format to write.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSaveImage(
                    FlyCaptureContext           context,
                    const FlyCaptureImage*      pimage,
                    const char*                 pszPath,
                    FlyCaptureImageFileFormat   format );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureConvertImage() 
//
// Description: 
//   Convert an arbitrary image format to another format.
//
// Arguments:
//   context      - The FlyCapture context to access.
//   pimageSrc    - The source image to convert
//   pimageDest   - The destination image to convert.  The pData member must be
//                  initialized to an output buffer of sufficient size, and
//                  the pixelFormat member indicates the desired output format.  
//                  Only BGR and BGRU are currently supported.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   This function replaces flycaptureConvertToBGR24(), 
//   flycaptureStippledToBGR24(), and flycaptureStippledToBGRU32().
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureConvertImage( 
                       FlyCaptureContext        context,
                       const FlyCaptureImage*   pimageSrc,
                       FlyCaptureImage*         pimageDest );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureInplaceRGB24toBGR24() 
//
// Description: 
//   Changes the input image buffer from 24-bit RGB to windows-displayable
//   24-bit BGR.
//
// Arguments:
//   pImageBuffer - Pointer to the image contents.
//   iImagePixels - Size of the image, in pixels.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureInplaceRGB24toBGR24( 
                              unsigned char* pImageBuffer,
                              int            iImagePixels );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureConvertToBGR24() 
//
// Description: 
//   Deprecated.  Please use flycaptureConvertImage().
//
// Arguments:
//   pimage       - A pointer to the image to convert.
//   pDestBuffer  - Pointer to an allocated buffer to hold the resultant image.
//                  This buffer must be at least iImagePixels * 3 bytes long.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   Note that passing an RGB image into this function will work, but it is much
//   more efficient to just call flycaptureInplaceRGB24toBGR24().
//   Y8 and Y16 images are converted to BGR24 greyscale.  Note that in the
//   case of Y16 this is a lossy conversion.
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureConvertToBGR24( 
			 const FlyCaptureImage* pimage,
			 unsigned char*         pDestBuffer );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureStippledToBGR24() 
//
// Description: 
//   Deprecated.  Please use flycaptureConvertImage().
//
// Arguments:
//   context	     - The FlyCapture context.
//   pimage	     - A pointer to the image to convert.
//   pDestBuffer     - Pointer to an allocated buffer to hold the resultant 
//                     image. This buffer must be at least iRows * iCols * 3
//                     bytes in size.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//  This function is only relevant to cameras that do not do onboard colour 
//  processing such as older versions of the Dragonfly.  This function uses the 
//  color method set in flycaptureSetColorProcessingMethod() and the stippled
//  format used in flycaptureSetColorTileFormat().
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureStippledToBGR24(
			  FlyCaptureContext        context,   
			  const FlyCaptureImage*   pimage,
			  unsigned char*           pDestBuffer );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureStippledToBGRU32() 
//
// Description: 
//   Deprecated.  Please use flycaptureConvertImage().
//
// Arguments:
//   context	     - The FlyCapture context.
//   pimage	     - A pointer to the image to convert.
//   pDestBuffer     - Pointer to an allocated buffer to hold the resultant 
//                     image. This buffer must be at least iRows * iCols * 4
//                     bytes in size.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//  This function is only relevant to cameras that do not do onboard colour 
//  processing such as older versions of the Dragonfly.  This function uses the 
//  color method set in flycaptureSetColorProcessingMethod() and the stippled
//  format used in flycaptureSetColorTileFormat().
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureStippledToBGRU32(
                           FlyCaptureContext        context,   
                           const FlyCaptureImage*   pimage,
                           unsigned char*           pDestBuffer );


//-----------------------------------------------------------------------------
//
// Name: flycaptureInplaceWhiteBalance()
//
// Description:
//   This function performs an inplace software based white balance on the 
//   provided image.
//
// Arguments:
//   context   - The FlyCapture context.
//   pData     - The BGR24 image data.
//   iRows     - Image rows.
//   iCols     - Image columns.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//  The image must be in BGR24 format.  I.e., the output from one of the above
//  functions.  This function has no effect on cameras that are detected to
//  have hardware whitebalance.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureInplaceWhiteBalance(
			      FlyCaptureContext context,
			      unsigned char*	pData,
			      int   		iRows,
			      int   		iCols );


//=============================================================================
// Camera Property Functions
//=============================================================================
// Group = Camera Property Functions

//-----------------------------------------------------------------------------
//
// Name:  flycaptureGetCameraPropertyRange() 
//
// Description:
//   Allows the user to examine the default, minimum, maximum, and auto 
//   characteristics for the given property.
//
// Arguments:
//   context        - The FlyCapture context to extract the properties from.
//   cameraProperty - A FlyCaptureProperty indicating the property to 
//                    examine.
//   pbPresent	    - A pointer to a bool that will contain whether or not 
//                    camera property is present.
//   plMin          - A pointer to a long that will contain the minimum 
//                    property value.
//   plMax          - A pointer to a long that will contain the maximum 
//                    property value.
//   plDefault      - A pointer to a long that will contain the default 
//                    property value.
//   pbAuto	    - A pointer to a bool that will contain whether or not
//                    the Auto setting is available for this property.
//   pbManual	    - A pointer to a bool that will contain whether or not
//                    this property may be manually adjusted.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   Pass NULL for any pointer argument to ignore that argument.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetCameraPropertyRange(
				 FlyCaptureContext  context,
				 FlyCaptureProperty cameraProperty,
				 bool*              pbPresent,
				 long*              plMin,
				 long*              plMax,
				 long*              plDefault,
				 bool*              pbAuto,
				 bool*		    pbManual );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureGetCameraProperty() 
//
// Description:
//   Allows the user to query the current value of the given property.
//
// Arguments:
//   context        - The FlyCapture context to extract the properties from.
//   cameraProperty - A FlyCaptureProperty indicating the property to 
//                    query.
//   plValueA       - A pointer to storage space for the "A", or first value
//                    associated with this property.
//   plValueB       - A pointer to storage space for the "B", or second value
//                    associated with this property.
//   pbAuto         - A pointer to a bool that will store the current Auto 
//                    value of the property.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   Pass NULL for any pointer argument to ignore that argument.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetCameraProperty(
			    FlyCaptureContext   context,
			    FlyCaptureProperty  cameraProperty,
			    long*               plValueA,
			    long*               plValueB,
			    bool*               pbAuto );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureSetCameraProperty() 
//
// Description:
//   Allows the user to set the given property.
//
// Arguments:
//   context        - The FlyCaptureContext to set the properties in.
//   cameraProperty - A FlyCaptureProperty indicating the property to set.
//   lValueA        - A long containing the "A", or first new value of the 
//                    property.
//   lValueB        - A long containing the "B", or second new value of the 
//                    property.
//   bAuto          - A boolean containing the new 'auto' state of the property.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   Calling this function with either of FLYCAPTURE_SOFTWARE_WHITEBALANCE
//   as the cameraProperty parameter and 'true' for the bAuto parameter will 
//   invoke a single shot auto white balance method.  The assumption is that 
//   flycaptureGrabImage() has been called previously with a white object 
//   centered in the field of view.  This will only work if the camera is a 
//   color camera and in RGB mode.  The Red and Blue whitebalance parameters 
//   only affect cameras that do offboard color calculation such as the 
//   Dragonfly.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetCameraProperty(
			    FlyCaptureContext  context,
			    FlyCaptureProperty cameraProperty,
			    long               lValueA,
			    long               lValueB,
			    bool               bAuto );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureSetCameraPropertyBroadcast() 
//
// Description:
//   Allows the user to set the given property for all cameras on the bus.
//
// Arguments:
//   context        - The FlyCaptureContext to set the properties in.
//   cameraProperty - A FlyCaptureProperty indicating the property to set.
//   lValueA        - A long containing the "A", or first new value of the 
//                    property.
//   lValueB        - A long containing the "B", or second new value of the 
//                    property.
//   bAuto          - A boolean containing the new 'auto' state of the property.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   This function will set the given property for all the cameras on the
//   1394 bus.  If you are using multiple busses (ie, more than one 1394 card)
//   you must call this function for each bus, on a context representing a 
//   camera on that bus.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetCameraPropertyBroadcast(
				     FlyCaptureContext  context,
				     FlyCaptureProperty cameraProperty,
				     long               lValueA,
				     long               lValueB,
				     bool               bAuto );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureGetCameraPropertyRangeEx() 
//
// Description:
//   Replaces flycaptureGetCameraPropertyRange() and provides better access to 
//   camera features.
//
// Arguments:
//   context        - The FlyCapture context to extract the properties from.
//   cameraProperty - A FlyCaptureProperty indicating the property to 
//                    examine.
//   pbPresent      - Indicates the presence of this property on the camera.
//   pbOnePush      - Indicates the availability of the one push feature.
//   pbReadOut      - Indicates the ability to read out the value of this property.
//   pbOnOff        - Indicates the ability to turn this property on and off.
//   pbAuto         - Indicates the availability of auto mode for this property.
//   pbManual       - Indicates the ability to manually control this property.
//   piMin          - The minimum value of the property is returned in this 
//                    argument.
//   piMax          - The maximum value of the property is returned in this 
//                    argument.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   Pass NULL for any pointer argument to ignore that argument.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetCameraPropertyRangeEx(
                                   FlyCaptureContext    context,
                                   FlyCaptureProperty   cameraProperty,
                                   bool*                pbPresent,
                                   bool*                pbOnePush,
                                   bool*                pbReadOut,
                                   bool*                pbOnOff,
                                   bool*                pbAuto,
                                   bool*		pbManual,
                                   int*                 piMin,
                                   int*                 piMax );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureGetCameraPropertyEx() 
//
// Description:
//   Replaces flycaptureGetCameraProperty() and provides better access to 
//   camera features.
//
// Arguments:
//   context        - The FlyCapture context to extract the properties from.
//   cameraProperty - A FlyCaptureProperty indicating the property to 
//                    query.
//   pbOnePush      - The value of the one push bit.
//   pbOnOff        - The value of the On/Off bit.
//   pbAuto         - The value of the Auto bit.
//   piValueA       - The current value of this property.
//   piValueB       - The current secondary value of this property. (only
//                    used for the two whitebalance values.)
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   Pass NULL for any pointer argument to ignore that argument.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetCameraPropertyEx(
                              FlyCaptureContext   context,
                              FlyCaptureProperty  cameraProperty,
                              bool*               pbOnePush,
                              bool*               pbOnOff,
                              bool*               pbAuto,
                              int*                piValueA,
                              int*                piValueB );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureSetCameraPropertyEx() 
//
// Description:
//   Replaces flycaptureSetCameraPropertyEx() and provides better access to 
//   camera features.
//
// Arguments:
//   context        - The FlyCaptureContext to set the properties in.
//   cameraProperty - A FlyCaptureProperty indicating the property to set.
//   bOnePush       - Set the one push bit.
//   bOnOff         - Set the on/off bit.
//   bAuto          - Set the auto bit.
//   iValueA        - The value to set.
//   iValueB        - The secondary value to set.  (only used for the two
//                    whitebalance values.)
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetCameraPropertyEx(
                              FlyCaptureContext    context,
                              FlyCaptureProperty   cameraProperty,
                              bool                 bOnePush,
                              bool                 bOnOff,
                              bool                 bAuto,
                              int                  iValueA,
                              int                  iValueB );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureSetCameraPropertyBroadcastEx() 
//
// Description:
//   Replaces flycaptureSetCameraPropertyBroadcast() and provides better access 
//   to camera features.
//
// Arguments:
//   context        - The FlyCaptureContext to set the properties in.
//   cameraProperty - A FlyCaptureProperty indicating the property to set.
//   bOnePush       - Set the one push bit.
//   bOnOff         - Set the on/off bit.
//   bAuto          - Set the auto bit.
//   iValueA        - The value to set.
//   iValueB        - The secondary value to set.  (only used for the two
//                    whitebalance values.)
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   This function will set the given property for all the cameras on the
//   1394 bus.  If you are using multiple busses (ie, more than one 1394 card)
//   you must call this function for each bus, on a context representing a 
//   camera on that bus.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetCameraPropertyBroadcastEx(
                                       FlyCaptureContext    context,
                                       FlyCaptureProperty   cameraProperty,
                                       bool                 bOnePush,
                                       bool                 bOnOff,
                                       bool                 bAuto,
                                       int                  iValueA,
                                       int                  iValueB );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureGetCameraAbsPropertyRange() 
//
// Description:
//   Allows the user to determine the presence and range of the absolute value
//   registers for the camera
//
// Arguments:
//   context - The Flycapture context to query.
//   cameraProperty - A FlyCaptureProperty indicating which property to query.
//   pbPresent - Whether or not this register has absolute value support.
//   pfMin - The minimum value that this register can handle.
//   pfMax - The maximum value that this register can handle.
//   ppszUnits - A string indicating the units of the register.
//   ppszUnitAbbr - An abbreviation of the units
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetCameraAbsPropertyRange(
				    FlyCaptureContext  context,
				    FlyCaptureProperty cameraProperty,
				    bool*              pbPresent,
				    float*             pfMin,
				    float*             pfMax,
				    const char**       ppszUnits,
				    const char**       ppszUnitAbbr );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureGetCameraAbsProperty() 
//
// Description:
//   Allows the user to get the current absolute value for a given parameter
//   from the camera if it is supported.
//
// Arguments:
//   context - The FlyCapture context to query.
//   cameraProperty - A FlyCaptureProperty indicating which property to query.
//   pfValue - A pointer to a float that will contain the result.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the operation.
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureGetCameraAbsProperty(
                               FlyCaptureContext   context,
                               FlyCaptureProperty  cameraProperty,
                               float*              pfValue );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureSetCameraAbsProperty() 
//
// Description:
//   Allows the user to set the absolute value of the given parameter if the 
//   mode is supported.
//
// Arguments:
//   context - The FlyCapture context to query.
//   cameraProperty - A FlyCaptureProperty indicating which property to query.
//   fValue - A float containing the new value of the parameter.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the operation.
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureSetCameraAbsProperty(
			       FlyCaptureContext  context,
			       FlyCaptureProperty cameraProperty,
			       float              fValue );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureSetCameraAbsPropertyBroadcast() 
//
// Description:
//   Allows the user to set the absolute value of the given parameter to all
//   cameras on the current bus.
//
// Arguments:
//   context - The FlyCapture context to query.
//   cameraProperty - A FlyCaptureProperty indicating which property to query.
//   fValue - A float containing the new value of the parameter.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the operation.
//
PGRFLYCAPTURE_API FlyCaptureError 
flycaptureSetCameraAbsPropertyBroadcast(
                                        FlyCaptureContext  context,
                                        FlyCaptureProperty cameraProperty,
                                        float              fValue );


//-----------------------------------------------------------------------------
// Name:  flycaptureGetCameraRegister()
//
// Description:
//   This function allows the user to get any of camera's registers.
//
// Arguments:
//   context    - The FlyCaptureContext associated with the camera to be queried.
//   ulRegister - The 32 bit register location to query.
//   pulValue   - The 32 bit value currently stored in the register.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetCameraRegister(
			    FlyCaptureContext context,
			    unsigned long     ulRegister,
			    unsigned long*    pulValue );


//-----------------------------------------------------------------------------
// Name:  flycaptureSetCameraRegister()
//
// Description:
//   This function allows the user to get any of the camera's registers.
//
// Arguments:
//   context    - The FlyCaptureContext associated with the camera to be queried.
//   ulRegister - The 32 bit register location to set.
//   ulValue    - The 32 bit value to store in the register.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetCameraRegister(
			    FlyCaptureContext context,
			    unsigned long     ulRegister,
			    unsigned long     ulValue );


//-----------------------------------------------------------------------------
// Name:  flycaptureSetCameraRegisterBroadcast()
//
// Description:
//   This function allows the user to get any register for all cameras on 
//   the bus.
//
// Arguments:
//   context    - The FlyCaptureContext associated with the camera to be queried.
//   ulRegister - The 32 bit register location to set.
//   ulValue    - The 32 bit value to store in the register.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetCameraRegisterBroadcast(
				     FlyCaptureContext context,
				     unsigned long     ulRegister,
				     unsigned long     ulValue );


//-----------------------------------------------------------------------------
// Name: flycaptureGetCameraTrigger()
//
// Description:
//   Deprecated.  Please use flycaptureGetTrigger().
//
// Returns:
//   FLYCAPTURE_DEPRECATED.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetCameraTrigger(
			   FlyCaptureContext context,
			   unsigned int*     puiPresence,
			   unsigned int*     puiOnOff,
			   unsigned int*     puiPolarity,
			   unsigned int*     puiTriggerMode );


//-----------------------------------------------------------------------------
// Name: flycaptureSetCameraTrigger()
//
// Description:
//    Deprecated.  Please use flycaptureSetTrigger().
//
// Returns:
//   FLYCAPTURE_DEPRECATED.
//  
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetCameraTrigger(
			   FlyCaptureContext context,
			   unsigned int      uiOnOff,
			   unsigned int	     uiPolarity,
			   unsigned int      uiTriggerMode );


//-----------------------------------------------------------------------------
// Name: flycaptureSetCameraTriggerBroadcast()
//
// Description:
//   Deprecated.  Please use flycaptureSetTriggerBroadcast().
//
// Returns:
//   FLYCAPTURE_DEPRECATED.
//
//  
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetCameraTriggerBroadcast(
				    FlyCaptureContext context,
				    unsigned char     ucOnOff,
				    unsigned char     ucPolarity,
				    unsigned char     ucTriggerMode );


//-----------------------------------------------------------------------------
// Name: flycaptureQueryTrigger()
//
// Description:
//   This function allows the user to query the trigger functionality of the
//   camera.
//
// Arguments:
//   context	     - The context associated with the camera to be queried.
//   pbPresent       - Whether or not the camera has trigger functionality.
//   pbReadOut       - Whether or not the user can read values in the trigger 
//                     functionality.
//   pbOnOff         - Whether or not the functionality can be turned on or 
//                     off.
//   pbPolarity      - Whether or not the polarity can be changed.
//   pbValueRead     - Whether or not the raw trigger input can be read.
//   puiSourceMask   - A bit field indicating which trigger sources are available.
//   pbSoftwareTrigger  - Whether or not software triggering is available.
//   puiModeMask     - A bit field indicating which trigger modes are available.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   Polarity and trigger sources are camera dependant.
//  
PGRFLYCAPTURE_API FlyCaptureError
flycaptureQueryTrigger(
                       FlyCaptureContext  context,
                       bool*              pbPresent,
                       bool*              pbReadOut,
                       bool*              pbOnOff,
                       bool*              pbPolarity,
                       bool*              pbValueRead,
                       unsigned int*      puiSourceMask,
                       bool*              pbSoftwareTrigger,
                       unsigned int*      puiModeMask );


//-----------------------------------------------------------------------------
// Name: flycaptureGetTrigger()
//
// Description:
//   This function allows the user to query the state of the camera's trigger 
//   functionality.  This function replaces the deprecated 
//   flycaptureGetCameraTrigger() function.
//
// Arguments:
//   context      - The context associated with the camera to be queried.
//   pbOnOff      - The On/Off state is returned in this parameter.
//   piPolarity   - The polarity value is returned in this parameter.
//   piSource     - The source value is returned in this parameter.
//   piRawValue   - The raw signal value is returned in this parameter.
//   piMode       - The trigger mode is returned in this parameter.
//   piParameter  - The parameter for the trigger function 
//                  is returned in this parameter.
//  
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetTrigger(
                     FlyCaptureContext context,
                     bool*             pbOnOff,
                     int*              piPolarity,
                     int*              piSource,
                     int*              piRawValue,
                     int*              piMode,
                     int*              piParameter );


//-----------------------------------------------------------------------------
// Name: flycaptureSetTrigger()
//
// Description:
//   This function allows the user to set the state of the camera's 
//   trigger functionality.  THis function replaces the deprecated 
//   flycaptureSetCameraTrigger() function.
//
// Arguments:
//   context	  - The context associated with the camera to be queried.
//   bOnOff       - Turn the trigger on or off.
//   iPolarity    - The polarity of the trigger. 1 or 0.
//   iSource      - The new trigger source.  Corresponds to the source mask.
//   iMode        - The new trigger mode.  Corresponds to the mode mask.
//   iParameter   - The (optional) parameter to the trigger function, if required.
// 
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//  If you have set a grab timeout using flycaptureSetGrabTimeoutEx(), this
//  timeout will be used in asynchronous trigger mode as well:  
//  flycaptureGrabImage*() will return with the image when you either trigger
//  the camera, or the timeout value expires.
//  
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetTrigger(
                     FlyCaptureContext context,
                     bool              bOnOff,
                     int               iPolarity,
                     int               iSource, 
                     int               iMode,
                     int               iParameter );


//-----------------------------------------------------------------------------
// Name: flycaptureSetTriggerBroadcast()
//
// Description:
//   This function duplicates the functionality of flycaptureSetTrigger, except
//   it broadcasts changes to all cameras on the bus.
//
// Arguments:
//   context	  - The context associated with the camera to be queried.
//   bOnOff       - Turn the trigger on or off.
//   iPolarity    - The polarity of the trigger. 1 or 0.
//   iSource      - The new trigger source.  Corresponds to the source mask.
//   iMode        - The new trigger mode.  Corresponds to the mode mask.
//   iParameter   - The (optional) parameter to the trigger function, if required.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//  
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetTriggerBroadcast(
                              FlyCaptureContext context,
                              bool              bOnOff,
                              int               iPolarity,
                              int               iSource, 
                              int               iMode,
                              int               iParameter );


//-----------------------------------------------------------------------------
// Name: flycaptureSetGrabTimeout()
//
// Description:
//   Deprecated.  Please use flycaptureSetGrabTimeoutEx().
//
// Arguments:
//   context - The FlyCaptureContext associated with the camera to be queried.
//   uiTimeout - The timeout value, in milliseconds.  A value of zero 
//               indicates an infinite wait.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetGrabTimeout(
			 FlyCaptureContext   context,
			 unsigned int	     uiTimeout );


//-----------------------------------------------------------------------------
// Name: flycaptureSetGrabTimeoutEx()
//
// Description:
//   This function allows the user to set the timeout value for 
//   flycaptureGrabImage*() and flycaptureLockLatest().  This is not normally 
//   necessary.
//
// Arguments:
//   context - The FlyCaptureContext associated with the camera to be queried.
//   uiTimeout - The timeout value, in milliseconds.  A value of 
//               FLYCAPTURE_INFINITE indicates an infinite wait.  A value of
//               zero indicates a nonblocking grab call.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//  The default grab timeout value is "infinite."  It is not normally necessary
//  to set this value.
//  
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetGrabTimeoutEx(
                           FlyCaptureContext context,
                           unsigned int	     uiTimeout );


#ifdef __cplusplus
};
#endif

#endif // #ifndef __PGRFLYCAPTURE_H__
