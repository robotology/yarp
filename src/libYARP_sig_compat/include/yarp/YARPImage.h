// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
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
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa, paulfitz#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPImage.h,v 1.3 2006-05-15 15:57:59 eshuy Exp $
///
///

#ifndef YARPImage_INC
#define YARPImage_INC

#ifdef __cplusplus

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

///
/// this is required to ship images through ports.
#include <yarp/YARPNetworkTypes.h>

#include <yarp/cvbase.h>

#endif /// __cplusplus

#ifndef YARP_IMAGE_HEADER_CONTROL

// the image types partially reflect the IPL image types.
// IPL allows 16/32bpp images. Should we constrain our lib to 8bpp.
// There must be a pixel type for every ImageType entry.
// this enumeration is also required in C code.
enum __PixelTypesEnum
    {	 
        YARP_PIXEL_INVALID = 0,
        YARP_PIXEL_MONO,
        YARP_PIXEL_RGB,
        YARP_PIXEL_HSV,
        YARP_PIXEL_BGR,  // external libraries sometimes want flipped order
        YARP_PIXEL_MONO_SIGNED,
        YARP_PIXEL_RGB_SIGNED,
        YARP_PIXEL_MONO_FLOAT,
        YARP_PIXEL_RGB_FLOAT,
        YARP_PIXEL_HSV_FLOAT,
        YARP_PIXEL_INT,
        // negative ids reserved for pixels of undeclared type but known size
        // in bytes
    };

#endif // YARP_IMAGE_HEADER_CONTROL

//
//
// This is required since matlab libraries include from C and need this
// file to have definition of data types.
#ifdef __cplusplus

#include <yarp/begin_pack_for_net.h>

// Pasa: had to change the pixel type names because they would clash
//	with the old lib names.

typedef unsigned char YarpPixelMono;

typedef NetInt32 YarpPixelInt;

struct YarpPixelRGB
{ 
    unsigned char r,g,b; 
  
    YarpPixelRGB() { r = g = b = 0; }
    YarpPixelRGB(unsigned char n_r, unsigned char n_g, unsigned char n_b)
    { r = n_r;  g = n_g;  b = n_b; }
} PACKED_FOR_NET;

struct YarpPixelBGR
{ 
    unsigned char b,g,r; 
    YarpPixelBGR() { b = g = r = 0; }
    YarpPixelBGR(unsigned char n_r, unsigned char n_g, unsigned char n_b)
    { r = n_r;  g = n_g;  b = n_b; }
} PACKED_FOR_NET;

typedef struct { unsigned char h,s,v; } YarpPixelHSV;
typedef char YarpPixelMonoSigned;
typedef struct { char r,g,b; } YarpPixelRGBSigned;
typedef float YarpPixelFloat;
typedef struct { float r,g,b; } YarpPixelRGBFloat;
typedef struct { float h,s,v; } YarpPixelHSVFloat;

#include <yarp/end_pack_for_net.h>

class YARPRefCount;

///
/// alignment required for IP, MMX, optimized stuff.
/// same alignment is required across platforms for communication issues.
const int YarpImageAlign = YARP_IMAGE_ALIGN;


// Note on IPL alignment stuff:
//	- initial address is aligned on a QUADWORD boundary (allocation).
//	- lines are extended to QUADWORD boundary.
//	- the initial alignment is mostly transparent.
//	- NT: memory allocated by IPL must be released within the same thread.
//
// Note on IPL memory allocation.
//	- Cleanup function is used to clean IPL allocated mem in a MT environment.
//	- A better memory allocator for IPL should be provided in the future.
//	- Use Cleanup before leaving the thread which allocated the image.

// Note on IPL alignment
//	- it's not required that all functions being compatible with IPL align.
//	- those which are not should assert if called for an image with
//		padding not equal to zero.
//	- most of the time image size is anyway a multiple of a QUADWORD.
//	- ex: 128 sq. images or typical 32x64 logpolar format.
//



// Note on Data double array. It's nice to have it because allows 
// transparently ignoring padding stuff. Simply access the image
// through row pointers... this should ease porting stuff from older 
// formats.
//

class YARPGenericImage;
void SatisfySize(YARPGenericImage& src, YARPGenericImage& dest);

/**
 * Basic image class.
 *
 * Objects of this class are able to create and manipulate images, but
 * has no real idea about their content.  For typed images, see
 * YARPImageOf.
 */
class YARPGenericImage
{
protected:
	int type_id;

	IplImage* pImage;
	char **Data;       // this is not IPL. it's char to maintain IPL compatibility

	YARPRefCount *buffer_references; // counts users of pImage->imageData
	int is_owner;

	// ipl allocation is done in two steps.
	// _alloc allocates the actual ipl pointer.
	// _alloc_data allocates the image array and data.
	// memory is allocated in a single chunk. Row ptrs are then
	// made to point appropriately. This is compatible with IPL and
	// SOMEONE says it's more efficient on NT.
	void _alloc (void);
	void _alloc_extern (void *buf);
	void _alloc_data (void);
	void _free (void);
	void _free_data (void);

	void _make_independent(); 
	void _set_ipl_header(int x, int y, int pixel_type);
	void _free_ipl_header();
	void _alloc_complete(int x, int y, int pixel_type);
	void _free_complete();

	void _alloc_complete_extern(void *buf, int x, int y, int pixel_type);

	// computes the # of padding bytes. These are always at the end of the row.
	int _pad_bytes (int linesize, int align) const;

public:

	/**
	 * Default constructor.
	 * Creates an empty image.
	 */
	YARPGenericImage(void);

	/**
	 * Copy constructor.
	 * Clones the content of another image.
	 * @param i the image to clone
	 */
	YARPGenericImage(const YARPGenericImage& i);

	/**
	 * Destructor.
	 */
	virtual ~YARPGenericImage();

	/**
	 * Resets image for IPL.
	 * This is just for IPL -- otherwise just use Clear().
	 * Discards all memory allocated for the image content.
	 * Cleanup function is used to clean IPL allocated mem
	 * in a multi-threaded environment.
	 * A better memory allocator for IPL should be provided in the future.
	 * Use Cleanup before leaving the thread which allocated the image.
	 */
	virtual void Cleanup () { _free_complete(); }

	/**
	 * Assignment operator.
	 * Clones the content of another image.
	 * @param i the image to clone
	 */
	virtual void operator=(const YARPGenericImage& i);

	/**
	 * Gets pixel type identifier.
	 * Images have an associated type identifier to
	 * permit automatic casting between different image types.
	 * @return the image type identifier
	 */
	virtual int GetID (void) const { return type_id; }


	/**
	 * Sets pixel type identifier.
	 * Only needed when working with non-native types.
	 * @param n_id the new pixel type identifier
	 */
	void SetID (int n_id) { type_id = n_id; }

	/**
	 * Sets pixel type identifier.
	 * Only needed when working with non-native types.
	 * @param n_id the new pixel type identifier
	 */
	void CastID (int n_id) { type_id = n_id; }

	/**
	 * Gets pixel size in memory in bytes.
	 * @return the size of the pixels stored in the image, in bytes
	 */
	int GetPixelSize (void) const;

	/**
	 * Gets height of image in pixels
	 * @return the height of the image in pixels (0 if no image present)
	 */
	inline int GetHeight() const 
	{ if (pImage!=NULL) return pImage->height; else return 0; }

	/**
	 * Gets width of image in pixels
	 * @return the width of the image in pixels (0 if no image present)
	 */
	inline int GetWidth() const  
	{ if (pImage!=NULL) return pImage->width;  else return 0; }

	/**
	 * Allocate memory for an image of a particular size and pixel type.
	 * If memory has already been allocated for an image of a different
	 * size/type, that is discarded.  This method is rarely needed -- use
	 * instead the Resize method of YARPImageOf, which knows which
	 * kind of pixels to make.
	 * @param the width of the image
	 * @param the height of the image
	 * @param the type identifier of the pixels
	 */
	void Resize (int x, int y, int pixel_type);

	/**
	 * Allocate memory for an image of a particular size.
	 * If memory has already been allocated for an image of a different
	 * size, that is discarded.  The type of the image is assumed to
	 * have been already decided, either by a call to SetID() or by
	 * using a derived class (YARPImageOf).
	 * @param the width of the image
	 * @param the height of the image
	 */
	void Resize (int x, int y) { Resize(x,y,GetID()); }

	/**
	 * Resets image.
	 * Discards all allocated memory.
	 */
	void Clear (void);

	/**
	 * Sets each pixel of the image to zero.
	 */
	inline void Zero (void) { ACE_ASSERT(pImage != NULL); memset(pImage->imageData, 0, pImage->imageSize); }

	/**
	 * Provide access to an external image stored in memory.
	 * Disables the built-in reference counting mechanism.
	 * After this call, the external image can be accessed using
	 * the same methods used to access normal YARP images.
	 * Need to check what this does about image padding! --
	 * be careful if your image rows are not quad-word aligned.
	 * @param buf the start of the memory block in which the image is stored
	 * @param x the width of the image
	 * @param y the height of the image
	 * @param pixel_type the type identifier of the pixels
	 */
	void UncountedRefer(void *buf, int x, int y, int pixel_type) { _alloc_complete_extern(buf,x,y,pixel_type); }

	/**
	 * Provide access to an external image stored in memory.
	 * Disables the built-in reference counting mechanism.
	 * After this call, the external image can be accessed using
	 * the same methods used to access normal YARP images.
	 * Need to check what this does about image padding! --
	 * be careful if your image rows are not quad-word aligned.
	 * @param buf the start of the memory block in which the image is stored
	 * @param x the width of the image
	 * @param y the height of the image
	 */
	void UncountedRefer(void *buf, int x, int y) { _alloc_complete_extern(buf,x,y,GetID()); }

	/**
	 * Provide access to another YARP image, without reference counting.
	 * @param src the image to access
	 */
	void UncountedRefer(const YARPGenericImage& src)
	{
		_alloc_complete_extern((void*)src.GetRawBuffer(), src.GetWidth(), src.GetHeight(), src.GetID());
	}

	/**
	 * Provide access to another YARP image, with reference counting.
	 * @param src the image to access
	 */
	void Refer(YARPGenericImage& src);


	/**
	 * Provide access to another YARP image, with reference counting.
	 * If the current image is typed differently to the other image,
	 * then a cast operation will occur, otherwise a Refer() will
	 * occur.
	 * @param src the image to access
	 */
	void ReferOrCopy(YARPGenericImage& src);

	/**
	 * Make a copy of another image, changing pixel type if needed.
	 * @src the image to copy
	 */
	void CastCopy(const YARPGenericImage& src);

	/**
	 * Make a copy of another image of the same pixel type.
	 * An error occurs if the image is of a different pixel type.
	 * Use this method if you want to be sure you are alerted
	 * to any unexpected casting occuring in your program.
	 * @src the image to copy
	 */
	void PeerCopy(const YARPGenericImage& src);

	/**
	 * Make a copy of another image, changing its size to the current
	 * image size.
	 * @src the image to copy
	 */
	void ScaledCopy(const YARPGenericImage& src);

	/**
	 * Make a scaled copy of another image.
	 * @src the image to copy
	 * @nx the desired final image width
	 * @ny the desired final image height
	 */
	void ScaledCopy(const YARPGenericImage& src, int nx, int ny);

	/**
	 * Extract a portion of the current image.
	 * Starting from a particular coordinate in the current image
	 * (startX,startY) this method fills a target image with 
	 * a rectangle from the current image.  The size of the
	 * rectangle is defined by the target image.
	 * @param id the target image
	 * @param startX the leftmost edge of the rectangle
	 * @param startY the upmost edge of the rectangle
	 */
	void Crop (YARPGenericImage& id, int startX, int startY);

	/**
	 * Overwrites a portion of the current image.
	 * Starting from a particular coordinate in the current image
	 * (startX,startY) this method copies a target image into
	 * a rectangular portion of the current image.  The size of the
	 * rectangle is defined by the target image.
	 * @param id the target image
	 * @param startX the leftmost edge of the rectangle
	 * @param startY the upmost edge of the rectangle
	 */
	void Paste (YARPGenericImage& is, int startX, int startY);

	/**
	 * Get a pointer to the underlying image buffer.
	 * Use at your own risk.
	 * @return a pointer to the underlying image buffer, if it exists, otherwise NULL
	 */
	char *GetRawBuffer() const { return (pImage!=NULL) ? pImage->imageData : NULL; }

	/**
	 * IPL cast operator
	 * @return underlying IPL-compatible structure, handy for calling IPL-compatible functions
	 */
	operator IplImage*() const { return pImage; }

	/**
	 * IPL cast operator
	 * @return underlying IPL-compatible structure, handy for calling IPL-compatible functions
	 */
	IplImage* GetIplPointer(void) const { return pImage; }

	/**
	 * Get array of row pointers.
	 * @return address of an array of row pointers
	 */
	inline char **GetArray(void) const { return (char **)Data; }

	/**
	 * Get a pointer to the underlying image buffer.
	 * Use at your own risk.
	 * @return a pointer to the underlying image buffer
	 */
	inline char *GetAllocatedArray(void) const { return (char*) pImage->imageData; }

	/**
	 * Size of the underlying image buffer.
	 * @return size of the underlying image buffer in bytes.
	 */
	inline int GetAllocatedDataSize(void) const { return pImage->imageSize; }

	/**
	 * Size of the underlying image buffer rows.
	 * @return size of the underlying image buffer rows in bytes.
	 */
	inline int GetAllocatedLineSize(void) const { return pImage->widthStep; }

	/**
	 * Size of the padding in underlying image buffer rows.
	 * Padding makes for more efficient access.
	 * @return number of bytes of padding of the underlying image buffer rows.
	 */
	// LATER: take into account pixel size also!
	inline int GetPadding() const { return _pad_bytes (pImage->width * pImage->nChannels, YarpImageAlign); }

	/**
	 * Find address of a pixel within the image.
	 * (0,0) is top-left of image.
	 * @param x the horizontal coordinate of the pixel, from 0 to GetWidth()-1
	 * @param y the vertical coordinate of the pixel, from 0 to GetHeight()-1
	 * @return the coordinate of the requested pixel.
	 */
	inline char *RawPixel(int x, int y) const { return (Data[y] + x*GetPixelSize()); }


	/**
	 * Make sure this image is bigger than another.
	 * Convenient during image processing.
	 * @param src the reference image
	 */
	void BiggerThan(YARPGenericImage& src) {
        SatisfySize(src,*this);
	}
};


/**
 * Typed image class.
 *
 * This is a wrapper over YARPGenericImage providing type security for
 * pixel access.
 */
template <class T>
class YARPImageOf : public YARPGenericImage
{
public:
	/**
	 * Copy constructor.
	 * @ i image to take a copy of
	 */
	YARPImageOf(const YARPImageOf<T>& i) : YARPGenericImage (i) {}

	/**
	 * Default constructor.
	 */
	YARPImageOf () : YARPGenericImage () {}

	/**
	 * Assignment operator.
	 * @ i image to take a copy of
	 */
	virtual void operator=(const YARPImageOf<T>& i) { YARPGenericImage::operator= (i); }

	// if type is not specialized, arbitrary id is specified that
	// carries size of pixel

	/**
	 * Get pixel type identifier.
	 * This is done automatically for recognized pixel types.
	 * @ return pixel type identifier
	 */
	virtual inline int GetID() const;

	/**
	 * Allocate memory for an image of a particular size.
	 * If memory has already been allocated for an image of a different
	 * size, that is discarded.
	 * @param the width of the image
	 * @param the height of the image
	 */
	void Resize(int x, int y) { YARPGenericImage::Resize(x,y,GetID()); }

	/**
	 * Empty pixel.
	 */
	T null_pixel;

	/**
	 * Access a pixel within the image.
	 * (0,0) is top-left of image.
	 * @param x the horizontal coordinate of the pixel, from 0 to GetWidth()-1
	 * @param y the vertical coordinate of the pixel, from 0 to GetHeight()-1
	 * @return reference to the requested pixel
	 */
	inline T& Pixel(int x, int y) { return *((T*)(Data[y] + x*sizeof(T))); }

	/**
	 * Access a pixel within the image. Returns reference to a const object.
	 * (0,0) is top-left of image.
	 * @param x the horizontal coordinate of the pixel, from 0 to GetWidth()-1
	 * @param y the vertical coordinate of the pixel, from 0 to GetHeight()-1
	 * @return a constant reference to the requested pixel
	 */
	inline const T& Pixel(int x, int y) const {return *((const T*)(Data[y] + x*sizeof(T))); }

	/**
	 * Access a pixel within the image.
	 * Identical to Pixel() method.
	 * @param x the horizontal coordinate of the pixel
	 * @param y the vertical coordinate of the pixel
	 * @return reference to the requested pixel
	 */
	T& operator()(int x, int y)	{ return Pixel(x,y); }

	/**
	 * Access a pixel within the image.
	 * Identical to Pixel(), const version
	 * @param x the horizontal coordinate of the pixel
	 * @param y the vertical coordinate of the pixel
	 * @return a constant reference to the requested pixel
	 */
	const T& operator()(int x, int y) const {return Pixel(x,y);};

	/**
	 * Access a pixel within the image, throwing an assertion if out-of-bounds.
	 * Otherwise identical to Pixel() method.
	 * @param x the horizontal coordinate of the pixel
	 * @param y the vertical coordinate of the pixel
	 * @return reference to the requested pixel
	 */
	T& AssertedPixel(int x, int y)
	{
		ACE_ASSERT(Data!=NULL && pImage != NULL);
		ACE_ASSERT(x >= 0 && 
                   x < pImage->width &&
                   y >= 0 &&
                   y < pImage->height);
		return Pixel(x,y);
	}

	/**
	 * Access a pixel within the image, or the null pixel if out-of-bounds.
	 * Otherwise identical to Pixel() method.
	 * Very convenient for avoiding excess conditionals on boundary
	 * conditions of otherwise homogeneous calculations across an
	 * image
	 * @param x the horizontal coordinate of the pixel
	 * @param y the vertical coordinate of the pixel
	 * @return reference to the requested pixel
	 */
	T& SafePixel(int x, int y)
	{
		ACE_ASSERT(Data != NULL && pImage != NULL);
		if (x >= 0 &&
			x < pImage->width &&
			y >= 0 &&
			y < pImage->height)  
			return Pixel(x,y);
		return null_pixel;
	}


	/**
	 * Check if coordinate is in range.
	 * @param x the horizontal coordinate of the pixel
	 * @param y the vertical coordinate of the pixel
	 * @return true is pixel (x,y) exists in the image
	 */
	bool IsSafePixel(int x, int y)
	{
		ACE_ASSERT(Data != NULL && pImage != NULL);
		if (x >= 0 &&
			x < pImage->width &&
			y >= 0 &&
			y < pImage->height)  
            return true;
		return false;
	}


	/**
	 * Access the null pixel.
	 * @return reference to the null pixel
	 */
	T& NullPixel() { return null_pixel; }

	/**
	 * Get an appropriately typed pointer to the underlying image buffer.
	 * @return a pointer to the underlying image buffer, if it exists, otherwise NULL
	 */
	T *GetTypedBuffer() { return (T*)GetRawBuffer(); }
};

template<class T>
inline int YARPImageOf<T>::GetID() const
{ return -((int)sizeof(T)); }

#define __YARPIMAGE_ASSOCIATE_TAG(tag,T) template<> inline int YARPImageOf<T>::GetID() const { return tag; }

__YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_MONO,YarpPixelMono)
    __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_RGB,YarpPixelRGB)
    __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_HSV,YarpPixelHSV)
    __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_BGR,YarpPixelBGR)
    __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_MONO_SIGNED,YarpPixelMonoSigned)
    __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_RGB_SIGNED,YarpPixelRGBSigned)
    __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_MONO_FLOAT,YarpPixelFloat)
    __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_RGB_FLOAT,YarpPixelRGBFloat)
    __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_HSV_FLOAT,YarpPixelHSVFloat)
    __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_INT,YarpPixelInt)

#undef __YARPIMAGE_ASSOCIATE_TAG

#include <yarp/YARPImagePort.h>

#endif	// of __cplusplus: this is required for Matlab code!

#ifndef YARP_IMAGE_HEADER_CONTROL
#define YARP_IMAGE_HEADER_CONTROL
#endif

#endif



    
