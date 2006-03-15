#ifndef _YARP2_IMAGE_
#define _YARP2_IMAGE_

#include <yarp/os/Portable.h>

namespace yarp {
  namespace sig {
    class Image;
    template <class T> class ImageOf;
  }
}


/**
 *
 * Class for storing images.  
 * This is a minimal class, designed to be as interoperable as possible
 * with other image classes in other libraries, particularly IPL-derived
 * libraries such as OpenCV.
 *
 */
class yarp::sig::Image : public yarp::os::Portable {

public:

  Image();

  virtual ~Image();

  inline int width() const { return imgWidth; }
  inline int height() const { return imgHeight; }

  virtual int getPixelSize() const;
  virtual int getPixelCode() const;

  inline int rowSize() const { return imgRowSize; }

  inline char *getPixelAddress(int x, int y) const {
    return data[y] + x*imgPixelSize;
  }

  inline bool isPixel(int x, int y) const {
    return (x>=0 && y>=0 && x<imgWidth && y<imgHeight);
  }

  void zero();

  void setPixelCode(int imgPixelCode);
  void setPixelSize(int imgPixelSize);
  void setRowSize(int imgRowSize);

  void resize(int imgWidth, int imgHeight);

  char *getRawImage();

  int getRawImageSize();

  /**
   * not yet implemented.
   * Returns IPL view of image, if possible.
   * Not possible if the image is the wrong size, with no padding.
   * @return pointer to an IplImage structure
   */
  void *getIplImage();

  /**
   * not yet implemented.
   * make sure to set all necessary properties first.
   */
  void wrapRawImage(void *buf, int imgWidth, int imgHeight);


  virtual bool read(ConnectionReader& connection);
  virtual bool write(ConnectionWriter& connection);

private:
  int imgWidth, imgHeight, imgPixelSize, imgRowSize, imgPixelCode;
  char **data;
  void *implementation;

  void synchronize();
};


#ifndef YARPImage_INC // old YARP header file that we should be compat with
#ifndef YARP_IMAGE_HEADER_CONTROL // interlocking with compatibility library

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

#endif
#endif

#include <yarp/os/begin_pack_for_net.h>

#include <yarp/os/NetInt32.h>

namespace yarp {
  namespace sig {

    typedef unsigned char PixelMono;

    typedef yarp::os::NetInt32 PixelInt;

    struct PixelRgb
    { 
      unsigned char r,g,b; 
      
      PixelRgb() { r = g = b = 0; }
      PixelRgb(unsigned char n_r, unsigned char n_g, unsigned char n_b)
      { r = n_r;  g = n_g;  b = n_b; }
    } PACKED_FOR_NET;
    
    struct PixelBgr
    { 
      unsigned char b,g,r; 
      PixelBgr() { b = g = r = 0; }
      PixelBgr(unsigned char n_r, unsigned char n_g, unsigned char n_b)
      { r = n_r;  g = n_g;  b = n_b; }
    } PACKED_FOR_NET;
    
    typedef struct { unsigned char h,s,v; } PixelHsv;
    typedef char PixelMonoSigned;
    typedef struct { char r,g,b; } PixelRgbSigned;
    typedef float PixelFloat;
    typedef struct { float r,g,b; } PixelRgbFloat;
    typedef struct { float h,s,v; } PixelHsvFloat;

#include <yarp/os/end_pack_for_net.h>

  }
}


/**
 * Typed image class.
 *
 * This is a wrapper over YARPGenericImage providing type security for
 * pixel access.
 */
template <class T>
class yarp::sig::ImageOf : public Image
{
private:
  T nullPixel;
public:
  virtual int getPixelSize() const {
    return sizeof(T);
  }

  virtual int getPixelCode() const {
    return -sizeof(T);
  }

  inline T& pixel(int x, int y) {
    return *((T *)(getPixelAddress(x,y)));
  }

  inline T& safePixel(int x, int y) {
    if (!isPixel(x,y)) { return nullPixel; }
    return *((T *)(getPixelAddress(x,y)));
  }
};

namespace yarp {
  namespace sig {

#define __YARPIMAGE_ASSOCIATE_TAG(tag,T) \
template<> \
class ImageOf<T> : public Image \
{ \
private: \
  T nullPixel; \
public: \
\
  virtual int getPixelSize() const { \
    return sizeof(T); \
  } \
\
  virtual int getPixelCode() const { \
    return tag; \
  } \
\
  inline T& pixel(int x, int y) { \
    return *((T *)(getPixelAddress(x,y))); \
  } \
\
  inline T& safePixel(int x, int y) { \
    if (!isPixel(x,y)) { return nullPixel; } \
    return *((T *)(getPixelAddress(x,y))); \
  } \
};


  __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_MONO,PixelMono)
  __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_RGB,PixelRgb)
  __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_HSV,PixelHsv)
  __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_BGR,PixelBgr)
  __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_MONO_SIGNED,PixelMonoSigned)
  __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_RGB_SIGNED,PixelRgbSigned)
  __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_MONO_FLOAT,PixelFloat)
  __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_RGB_FLOAT,PixelRgbFloat)
  __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_HSV_FLOAT,PixelHsvFloat)
  __YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_INT,PixelInt)

  }
}

#undef __YARPIMAGE_ASSOCIATE_TAG


#ifndef YARP_IMAGE_HEADER_CONTROL
#define YARP_IMAGE_HEADER_CONTROL
#endif

#endif
