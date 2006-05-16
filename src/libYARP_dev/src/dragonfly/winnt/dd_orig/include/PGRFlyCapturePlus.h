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
// PGRFlyCapturePlus.h
//
//   Defines advanced functionality of the FlyCapture SDK.  Please note that 
//   these functions are for advanced users only and that regular users need
//   not care about this file.
//
//  We welcome your bug reports, suggestions, and comments: 
//  www.ptgrey.com/support/contact
//
//=============================================================================

//=============================================================================
// $Id: PGRFlyCapturePlus.h,v 1.1 2006-05-16 09:57:57 natta Exp $
//=============================================================================
#ifndef __PGRFLYCAPTUREPLUS_H__
#define __PGRFLYCAPTUREPLUS_H__


#ifdef __cplusplus
extern "C"
{
#endif


//
// Description:
//  A wrapper for FlyCaptureImage that provides advanced functionality.
//
typedef struct FlyCaptureImagePlus
{
   //
   // The FlyCaptureImage that this FlyCaptureImagePlus sturcture is wrapping.
   // Please see documentation in pgrflycapture.h.
   //
   FlyCaptureImage   image;

   //
   // The sequence number of the image.  This number is generated in the 
   // driver and sequential images should have a difference of one.  If
   // the difference is greater than one, it indicates the number of missed
   // images since the last lock image call.
   //
   unsigned int      uiSeqNum;

   //
   // The internal buffer index that the image buffer contained in the
   // FlyCaptureImage corresponds to.  For functions that lock the image,
   // this number must be passed back to the "unlock" function.  If 
   // flycaptureInitializePlus() was called, this number corresponds to the 
   // position of the buffer in the buffer array passed in.
   //
   unsigned int      uiBufferIndex;

   //
   // Reserved for future use.
   //
   unsigned long     ulReserved[ 8 ];

} FlyCaptureImagePlus;


//
// Description:
//   This structure is used for partial image notification functionality.
//   Please see the release notes and API documentation for details.
//   
// See Also:
//   flycaptureInitializeNotify(), flycaptureLockNextEvent(), 
//   flycaptureWaitForImageEvent(), flycaptureUnlockEvent().
//   flycaptureGetPacketSize(), flycaptureGetCustomImageMaxPacketSize()
//
typedef struct FlyCaptureImageEvent
{
   //
   // A pointer to the start of the location inside the image buffer that 
   // this event corresponds to.  This is only valid on structures coming back
   // from flycaptureLockNextEvent() and points to a buffer allocated 
   // internally (in the case of flycaptureInitialize*(), or passed in by the
   // user (in the case of flycaptureInitializePlus()).
   //
   unsigned char* pBuffer;

   //
   // The size of the image portion that this event corresponds to.  This must
   // be specified for structures being passed in to 
   // flycaptureInitializeNotify().  This is the only member that needs to be
   // specified.  The sizes passed in to flycaptureInitializeNotify() must
   // add up to the total image size and must be whole multiples of the packet 
   // size.  Appropriate packet sizes can be determined using 
   // flycaptureGetPacketSize() and flycaptureGetCustomImagePacketInfo().
   //
   unsigned int   uiSizeBytes;    
   
   //
   // Sequence number for this image event.  Populated by 
   // flycaptureWaitForImageEvent().  Sequence numbers should be contiguous if
   // no image buffers are being dropped.  If they are not, then the user level
   // grab thread is not keeping up with the images the camera is sending, and
   // this is a fatal error.
   //
   unsigned int   uiSeqNum;

   // The internal buffer index of this image portion.  This can be ignored.
   unsigned int   uiBufferIndex;   
   
   // Internal bookkeeping.  This can be ignored.
   void*          pInternal;

   // Reserved for future use.
   unsigned long  ulReserved[ 8 ];
   
} FlyCaptureImageEvent;


//
// Description:
//  Packet size information.
//
typedef struct FlyCapturePacketInfo
{
   // Minimum packet size, in bytes.
   unsigned int   uiMinSizeBytes;
   // Maximum packet size, in bytes.  Note that this ignores the OS-enforced
   // bandwidth restrictions.  The realized max packet size will be 80% of
   // this reported value.
   unsigned int   uiMaxSizeBytes;
   // Maximum packet size, in bytes, when using events.  The bandwidth note
   // for uiMaxSizeBytes applies here too.
   unsigned int   uiMaxSizeEventBytes;
   // Reserved for future use.
   unsigned long  ulReserved[ 8 ];
   
} FlyCapturePacketInfo;


//
// Description:
//  Available image filters.  These are bit values for a bitmask that will be 
//  set with flycaptureSetImageFilers() and retrieved with 
//  flycaptureGetImageFilters().  Currently there is only one available filter.
//
enum
{
   // Disable all image filters.
   FLYCAPTURE_IMAGE_FILTER_NONE                 = 0x00000000,
   //
   // Crosstalk filter for colour Scorpion cameras with the 
   // Symmagery VCA1281 sensor.  This filter will be automatically
   // enabled for cameras with this sensor.  This filter is applied
   // during flycaptureStippledToBGR*() calls.
   //
   FLYCAPTURE_IMAGE_FILTER_SCORPION_CROSSTALK   = 0x00000001,
   // Enable all image filters.
   FLYCAPTURE_IMAGE_FILTER_ALL                  = 0xFFFFFFFF,
};


//-----------------------------------------------------------------------------
//
// Name:  flycaptureReadRegisterBlock()
//
// Description:
//   Provides block-read (asynchronous) access to the entire register space of
//   the camera.
//
// Arguments:
//   context    - The context associated with the camera to be accessed.
//   usAddrHigh - The top 16 bits of the 48-bit absolute address to read.
//   ulAddrLow  - The bottom 32 bits of the 48-bit absolute addresss to read.
//   pulBuffer  - The buffer that will receive the data.  Must be of size
//                ulLength.
//   ulLength   - The length, in quadlets, of the block to read.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureReadRegisterBlock(
			    FlyCaptureContext   context,
                            unsigned short      usAddrHigh,
                            unsigned long       ulAddrLow,
                            unsigned long*      pulBuffer,
                            unsigned long       ulLength );
                            

//-----------------------------------------------------------------------------
//
// Name:  flycaptureWriteRegisterBlock()
//
// Description:
//   Provides block-write (asynchronous) access to the entire register space of
//   the camera.
//
// Arguments:
//   context    - The context associated with the camera to be accessed.
//   usAddrHigh - The top 16 bits of the 48-bit absolute address to write.
//   ulAddrLow  - The bottom 32 bits of the 48-bit absolute addresss to write.
//   pulBuffer  - The buffer that contains the data to be written.
//   ulLength   - The length, in quadlets, of the block to write. 
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureWriteRegisterBlock(
                             FlyCaptureContext      context,
                             unsigned short         usAddrHigh,
                             unsigned long          ulAddrLow,
                             const unsigned long*   pulBuffer,
                             unsigned long          ulLength );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureInitializePlus()
//
// Description:
//   Identical behaviour to flycaptureInitialize(), except that the user has
//   the option of specifying the number of buffers to use, and optionally
//   allocate those buffers outside the library.
//
// Arguments:
//   context      - The context associated with the camera to be accessed.
//   uiBusIndex   - The zero-based device index of the camera to be initialized.
//   uiNumBuffers - The number of buffers to expect or allocate.  For lock next
//                  mode, the minimum number of buffers is 2.  For lock latest
//                  mode, the minimum number of buffers is 4.  The maximum 
//                  number of buffers is only limited by system memory.
//   arpBuffers   - An array of pointers to buffers.  If this argument is NULL
//                  the library will allocate and free the buffers internally,
//                  otherwise the caller is responsible for allocation and 
//                  deallocation.  No boundary checking is done on these 
//                  images, if you are supplying your own buffers, they must
//                  be large enough to hold the largest image you are 
//                  expecting.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   If you wish to use the camera serial number to initialize, or you don't 
//   care about the number of buffers being allocated, use either of the
//   other initialize methods in pgrflycapture.h.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureInitializePlus(
                         FlyCaptureContext   context,
                         unsigned int        uiBusIndex,
                         unsigned int        uiNumBuffers,
                         unsigned char**     arpBuffers );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureStartCustomImagePacket()
//
// Description:
//   This function is identical to flycaptureStartCustomImage() except it 
//   takes in a packet size rather than a float bandwidth parameter.
//
// Arguments:
//   context            - The FlyCaptureContext to start grabbing.
//   uiMode             - The camera-specific mode.  (0-7).
//   uiImagePosLeft     - The left position of the (sub)image.
//   uiImagePosTop      - Top top position of the (sub)image.
//   uiWidth            - The width of the (sub)image.
//   uiHeight           - The height of the (sub)image.
//   uiPacketSizeBytes  - The number of packets to send per isochronous period.
//                        A larger packet size will result in faster image 
//                        transmission and increased bandwidth requirements.
//                        This number should be a multiple of 4 and fit within
//                        the values defined by 
//                        flycaptureGetCustomImageMaxPacketSize().
//   format             - The pixel format to be used.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// See Also:
//   flycaptureStartCustomImage(), flycaptureQueryCustomImage(),
//   flycaptureGetCustomImagePacketInfo()
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureStartCustomImagePacket(
                                 FlyCaptureContext       context,
                                 unsigned int	         uiMode,
                                 unsigned int            uiImagePosLeft,
                                 unsigned int            uiImagePosTop,
                                 unsigned int	         uiWidth,
                                 unsigned int	         uiHeight,
                                 unsigned int	         uiPacketSizeBytes,
                                 FlyCapturePixelFormat   format );



//-----------------------------------------------------------------------------
//
// Name:  flycaptureStartLockNext()
//
// Description:
//   Starts the camera streaming and initializes the library for "lock next"
//   functionality.  This function needs to used instead of flycaptureStart()
//   for the following "lock next" functions.
//
// Arguments:
//   context   - The context associated with the camera to be started.
//   videoMode - The video mode to start the camera in.
//   frameRate - The frame rate to start the camera at.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   For "lock latest" functionality, use flycaptureStart() and the 
//   flycaptureLockLatest().
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureStartLockNext(
                        FlyCaptureContext    context,
                        FlyCaptureVideoMode  videoMode, 
                        FlyCaptureFrameRate  frameRate );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureStartLockNextCustomImage()
//
// Description:
//   This function is identical to flycaptureStartLockNext(), except that it 
//   will start the camera in custom image mode.  See 
//   flycaptureStartCustomImage().
//
// Arguments:
//   context         - The context associated with the camera to be started.
//   uiMode          - The camera-specific mode.  (0-7).
//   uiImagePosLeft  - The left position of the (sub)image.
//   uiImagePosTop   - Top top position of the (sub)image.
//   uiWidth         - The width of the (sub)image.
//   uiHeight        - The height of the (sub)image.
//   fBandwidth      - The bandwidth to assign to this camera.  100.0 indicates 
//                     full bandwidth.
//   format          - The pixel format to be used.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   For "lock latest" functionality, use flycaptureStart() and the 
//   flycaptureLockLatest().
//
// See Also:
//   flycaptureQueryCustomImage(), flycaptureStartCustomImage().
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureStartLockNextCustomImage(
                                   FlyCaptureContext     context,
                                   unsigned int	         uiMode,
                                   unsigned int          uiImagePosLeft,
                                   unsigned int          uiImagePosTop,
                                   unsigned int	         uiWidth,
                                   unsigned int	         uiHeight,
                                   float                 fBandwidth,
                                   FlyCapturePixelFormat format );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureStartLockNextCustomImagePacket()
//
// Description:
//   This function is identical to flycaptureStartLockNextCustomImage(), 
//   except that it takes a packet size in bytes, instead of a floating point 
//   bandwidth estimation.
//
// Arguments:
//   context         - The context associated with the camera to be started.
//   uiMode          - The camera-specific mode.  (0-7).
//   uiImagePosLeft  - The left position of the (sub)image.
//   uiImagePosTop   - Top top position of the (sub)image.
//   uiWidth         - The width of the (sub)image.
//   uiHeight        - The height of the (sub)image.
//   uiPacketSizeBytes  - The number of packets to send per isochronous period.
//                        A larger packet size will result in faster image 
//                        transmission and increased bandwidth requirements.
//                        This number should be a multiple of 4 and fit within
//                        the values defined by 
//                        flycaptureGetCustomImageMaxPacketSize().
//   format          - The pixel format to be used.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// See Also:
//   flycaptureQueryCustomImage(), flycaptureStartCustomImage().
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureStartLockNextCustomImagePacket(
                                         FlyCaptureContext     context,
                                         unsigned int	       uiMode,
                                         unsigned int          uiImagePosLeft,
                                         unsigned int          uiImagePosTop,
                                         unsigned int	       uiWidth,
                                         unsigned int	       uiHeight,
                                         unsigned int	       uiPacketSizeBytes,
                                         FlyCapturePixelFormat format );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureSyncForLockNext()
//
// Description:
//  Takes in an array of contexts attached to multiple cameras that 
//  are already synchronized in hardware (like Dragonflys) and assures that the 
//  next time lockNext() is called for all contexts, the images locked will 
//  correspond to one another.  Note that this function only needs to be called 
//  once after the contexts have been started.  The contexts should be started 
//  in the same order that they are listed in arContexts before this function 
//  is called.
//
// Arguments:
//   arContexts - An array of contexts attached to the cameras to synchronize.
//   uiContexts - The number of contexts in arContext.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   This function operates by skipping the appropriate number of images
//   in contexts that were started "after" the reference context (position 0
//   in the array).  If this function fails it does not necessarily mean the
//   cameras are out of sync.  This is still experimental.  Note also that
//   this function will turn on image timestamping.  Please contact PGR
//   support for more information.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSyncForLockNext(
                          FlyCaptureContext* arContexts,
                          unsigned int       uiContexts );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureLockNext()
//
// Description:
//   Lock the "next" image that has not been seen.  Provided that the previous 
//   image processing time is not greater than the time taken for the camera
//   to transmit images to the available unlocked buffers, this function can
//   be called repeatedly to guarantee that each image will be seen.  If the 
//   camera has not finished transmitting the next image, this function will 
//   block.  Users can verify image sequentiality by comparing sequence 
//   numbers of sequential images. 
//
// Arguments:
//   context   - The context associated with the camera to be accessed.
//   pimage    - The returned FlyCaptureImagePlus.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   flycaptureUnlock() must be called using the buffer index returned in 
//   pimage when processing on this image has been completed.  The camera must
//   have been started using flycaptureStartLockNext() for this function to
//   succeed.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureLockNext(
                   FlyCaptureContext      context,
                   FlyCaptureImagePlus*   pimage );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureLockLatest()
//
// Description:
//   Lock the "latest" image that has not been seen.  If there is an unseen 
//   image waiting, this function will return immediately with that image, 
//   otherwise it will block until the next image has been received.  The 
//   difference in the sequence numbers of images returned by consecutive calls
//   to this function indicates the number of missed images between calls.
//
// Arguments:
//   context - The context associated with the camera to be accessed.
//   pimage  - The returned FlyCaptureImagePlus.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   flycaptureUnlock() must be called using the buffer index returned in 
//   pimage when processing on this image has been completed.  This function
//   behaves identically to flycaptureGrabImage(), except it doesn't implicitly
//   unlock the previously seen image first.  The camera must have been 
//   started using flycaptureStart() in order for this function to succeed.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureLockLatest(
                     FlyCaptureContext    context,
                     FlyCaptureImagePlus* pimage );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureUnlock()
//
// Description:
//   Returns a buffer into the pool to be filled by the camera driver.  This 
//   must be called for each image locked using the above lock functions after
//   processing on that image has been completed.
//
// Arguments:
//   context       - The context associated with the camera to be accessed.
//   uiBufferIndex - The buffer to unlock.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureUnlock(
                 FlyCaptureContext  context,
                 unsigned int       uiBufferIndex );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureUnlockAll()
//
// Description:
//   Unlocks all locked images.  This is equivalent to maintaining a list of 
//   locked buffers and calling flycaptureUnlock() for each.
//
// Arguments:
//   context - The context associated with the camera to be accessed.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureUnlockAll(
                    FlyCaptureContext  context );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureGetImageFilters()
//
// Description:
//   Retrieves the currently active filters.  The returned number is a 
//   bitmap corresponding to the FLYCAPTURE_IMAGE_FILTER_* values.
//
// Arguments:
//   context    - The context associated with the camera to be accessed.
//   puiFilters - The filter bitmap is returned in this value.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetImageFilters(
			  FlyCaptureContext   context,
                          unsigned int*       puiFilters );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureSetImageFilters()
//
// Description:
//   Sets the active filters.  The returned number is a bitmap corresponding to
//   the FLYCAPTURE_IMAGE_FILTER_* values.
//
// Arguments:
//   context   - The context associated with the camera to be accessed.
//   uiFilters - The filters to set.  Use FLYCAPTURE_IMAGE_FILTER_NONE to
//               disable image filtering.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetImageFilters(
			  FlyCaptureContext   context,
                          unsigned int        uiFilters );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureGetImageTimestamping()
//
// Description:
//   Retrieves the status of camera-generated image timestamping.
//
// Arguments:
//   context - The context associated with the camera to be accessed.
//   pbOn    - Whether or not the camera is producing image timestamps.
//
// Returns:
//   FLYCAPTURE_OK - If the time stamping status was read correctly.
//   FLYCAPTURE_NOT_IMPLEMENTED - If the camera does not support image 
//                                timestamping.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetImageTimestamping(
                               FlyCaptureContext  context,
                               bool*              pbOn );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureSetImageTimestamping()
//
// Description:
//   Sets image timestamping.  If image timestamping is on, the first 4 bytes
//   of the image will contain camera-generated timestamp information, and 
//   the cycle seconds, count, and offset returned in FlyCaptureTimestamp
//   will use the data.
//
// Arguments:
//   context - The context associated with the camera to be accessed.
//   bOn     - On or off flag for image timestamping.
// 
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureSetImageTimestamping(
                               FlyCaptureContext  context,
                               bool               bOn );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureParseImageTimestamp()
//
// Description:
//   Parses the first 4 bytes of an image generated with image timestamping on
//   to retrieve 1394 timestamp information.
//
// Arguments:
//   context    - The context associated with the camera to be accessed.
//   pData      - The image data to be parsed.
//   puiSeconds - The seconds component of the 1394 timestamp.
//   puiCount   - The count component of the 1394 timestamp.
//   puiOffset  - The offset component of the 1394 timestamp.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureParseImageTimestamp(
                              FlyCaptureContext    context,
                              const unsigned char* pData,
                              unsigned int*        puiSeconds,
                              unsigned int*        puiCount,
                              unsigned int*        puiOffset );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureInitializeNotify()
//
// Description:
//   Initializes partial image notification mode.  
//   Partial image notification allows the grabbing program to be notified
//   several times during a single image grab.  Processing on an image can
//   then begin even before the entire image has been acquired.
//   This function must be called after a camera initialization function like 
//   flycaptureInitialize() or flycaptureInitializePlus(), and before a start 
//   function like flycaptureStartLockNext().  flycaptureLockNextEvent(), 
//   flycaptureWaitForImageEvent() and flycaptureUnlockEvent() are the only
//   image acquisition functions that can be used when in partial image
//   notification mode.  Please see the ImageEventEx example for more 
//   information.
//
// Arguments:
//   context     - The context associated with the camera to be accessed.
//   uiNumEvents - The number of desired image events per image.  The maximum
//                 number of events is camera-dependant.
//   arpEvents   - An array of uiNumEvents event structures.  The uiSizeBytes
//                 member must be filled, which indicates which portion of the
//                 image each event is for.  The image portions need not be 
//                 equal sized.  No other members need to be filled.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   This will not work unless you have a hotfix to the 1394 subsystem from 
//   Microsoft. Please see PGR knowledge base article 153, 
//   http://www.ptgrey.com/support/kb/details.asp?id=153, for more information.
//   Partial image notification is not available for "lock latest" 
//   functionality.  This is PGR bug 2126.
//
// See Also:
//   flycaptureGetPacketInfo(), flycaptureGetCustomImagePacketInfo()
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureInitializeNotify(
                           FlyCaptureContext    context,
                           unsigned int         uiNumEvents,
                           FlyCaptureImageEvent arpEvents[] );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureLockNextEvent()
//
// Description:
//   When in partial image notification mode (flycaptureInitializeNotify()),
//   this function will fill an array of FlyCaptureImageEvent structures 
//   corresponding to the requested events for each received image.  This
//   function will not block.
//
// Arguments:
//   context   - The context associated with the camera to be accessed.
//   pimage    - The returned FlyCaptureImage corresponding to the image that
//               the events are for. 
//   arpEvents - An array of event structures that will be filled by this 
//               function.  The number of events in this array must be
//               the number passed in to flycaptureInitializeNotify().  This
//               array can contain the same events that were passed into 
//               flycaptureInitializeNotify(), or it can be a new array if you
//               wish to retain ownership of the image buffer.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   This will not work unless you have a hotfix to the 1394 subsystem from 
//   Microsoft. Please see PGR knowledge base article 153, 
//   http://www.ptgrey.com/support/kb/details.asp?id=153, for more information.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureLockNextEvent(
                        FlyCaptureContext    context,
                        FlyCaptureImage*     pimage,
                        FlyCaptureImageEvent arpEvents[] );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureWaitForImageEvent()
//
// Description:
//   This function waits for a single partial image image event, as defined by 
//   the sizes specified by flycaptureInitializeNotify().  If the event has
//   already been triggered (the image part has already been received) this
//   function will return immediately.  It is not necessary to call this 
//   function for all the events in an image.  The events from a single image 
//   will be triggered in order.  To verify that no images have been missed,
//   call this function on all the events of all the images received and verify
//   the sequence numbers are contiguous.
//   
// Arguments:
//   context   - The context associated with the camera to be accessed.
//   pevent    - The event structure corresponding to the part of the image
//               to wait for.  This should be one of the structures filled in 
//               by flycaptureLockNextEvent().  At this point, the sequence
//               number of the event is filled.
//   uiTimeout - The time, in milliseconds, to wait for the image event to be 
//               received.  Can be FLYCAPTURE_INFINITE.
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   This will not work unless you have a hotfix to the 1394 subsystem from 
//   Microsoft. Please see PGR knowledge base article 153, 
//   http://www.ptgrey.com/support/kb/details.asp?id=153, for more information.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureWaitForImageEvent(
                            FlyCaptureContext      context,
                            FlyCaptureImageEvent*  pevent, 
                            unsigned int           uiTimeout );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureUnlockEvent()
//
// Description:
//   This function will release ownership of the buffers in the set of event
//   structures.  It has the same functionality as flycaptureUnlock(), except
//   that it unlocks the buffers in the correct order.
//
// Arguments:
//   context   - The context associated with the camera to be accessed.
//   arpEvents - An array of event structures that will be unlocked by this 
//               function.  The number of events in this array must be
//               the number passed in to flycaptureInitializeNotify().  This
//               array should be the same one filled by 
//               flycaptureLockNextEvent().
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// Remarks:
//   This will not work unless you have a hotfix to the 1394 subsystem from 
//   Microsoft. Please see PGR knowledge base article 153, 
//   http://www.ptgrey.com/support/kb/details.asp?id=153, for more information.
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureUnlockEvent(
                      FlyCaptureContext      context,
                      FlyCaptureImageEvent   arpEvents[] );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureGetPacketInfo()
//
// Description:
//   Returns the isochronous packet size for the indicated video mode and 
//   frame rate.  This number is useful when deciding the amount of data for 
//   each image event notification.  The size of each image event has to be a 
//   multiple of the packet size.  It is also useful for determining the amount
//   of bandwidth required to run a camera at a given mode and frame rate.
//
// Arguments:
//   context   - The context associated with the camera to be accessed.
//   videoMode - Required video mode.
//   frameRate - Required frame rate.
//   pinfo     - Returned packet size information.         
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// See Also:
//   flycaptureGetCustomImagePacketInfo()
//
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetPacketInfo(
                        FlyCaptureContext       context,
                        FlyCaptureVideoMode     videoMode,
                        FlyCaptureFrameRate     frameRate,
                        FlyCapturePacketInfo*   pinfo );


//-----------------------------------------------------------------------------
//
// Name:  flycaptureGetCustomImagePacketInfo()
//
// Description:
//   Returns isochronous packet size information for the indicated custom image
//   mode and image size.  The maximum packet size is useful for determining a 
//   minimum image event notification size.  This function is very similar to 
//   flycaptureGetPacketSize() but should be used when dealing with custom image 
//   modes.
//
// Arguments:
//   context   - The context associated with the camera to be accessed.
//   uiMode    - The camera-specific mode.  (0-7).
//   uiWidth   - The width of the (sub)image.
//   uiHeight  - The height of the (sub)image.
//   format    - The pixel format to be used.
//   pinfo     - Returned packet size information.         
//
// Returns:
//   A FlyCaptureError indicating the success or failure of the function.
//
// See Also:
//   flycaptureGetPacketInfo(), flycaptureStartCustomImagePacket()
// 
PGRFLYCAPTURE_API FlyCaptureError
flycaptureGetCustomImagePacketInfo(
                                   FlyCaptureContext     context,
                                   unsigned int	         uiMode,
                                   unsigned int	         uiWidth,
                                   unsigned int	         uiHeight,
                                   FlyCapturePixelFormat format,
                                   FlyCapturePacketInfo* pinfo );



#ifdef __cplusplus
};
#endif

#endif // #ifndef __PGRFLYCAPTUREPLUS_H__
