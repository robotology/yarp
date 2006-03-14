#ifndef _YARP2_IMAGE_
#define _YARP2_IMAGE_

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
class yarp::sig::Image {

public:

  Image();

  virtual ~Image();

  inline int width() const { return imgWidth; }
  inline int height() const { return imgHeight; }

  // these two properties vary by type - see ImageOf class
  virtual int pixelSize() const;
  virtual int pixelCode() const;

  inline int rowSize() const { return imgRowSize; }

  inline char *getPixelAddress(int x, int y) const {
    return data[y] + x*imgPixelSize;
  }

  void zero();

  void setPixelCode(int imgPixelCode);
  void setPixelSize(int imgPixelSize);
  void setRowSize(int imgRowSize);

  void resize(int imgWidth, int imgHeight);

  void *getRawImage();

  /**
   * Returns IPL view of image, if possible.
   * Not possible if the image is the wrong size, with no padding.
   * @return pointer to an IplImage structure
   */
  void *getIplImage();

  // make sure to set all necessary properties first
  void wrapRawImage(void *buf, int imgWidth, int imgHeight);

private:
  int imgWidth, imgHeight, imgPixelSize, imgRowSize, imgPixelCode;
  char **data;
  void *implementation;

  void synchronize();
};



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
public:
  virtual int pixelSize() const {
    return sizeof(T);
  }

  virtual int pixelCode() const {
    return -sizeof(T);
  }

  inline T& pixel(int x, int y) const {
    return *((T *)(getPixelAddress(x,y)));
  }
};

namespace yarp {
  namespace sig {

#define __YARPIMAGE_ASSOCIATE_TAG(tag,T) \
template<> \
class ImageOf<T> : public Image \
{ \
  virtual int pixelSize() const { \
    return sizeof(T); \
  } \
\
  virtual int pixelCode() const { \
    return tag; \
  } \
\
  inline T& pixel(int x, int y) const { \
    return *((T *)(getPixelAddress(x,y))); \
  } \
};


/*
template<class T>
inline int yarp::sig::ImageOf<T>::pixelCode() const
{ return -((int)sizeof(T)); }

#define __YARPIMAGE_ASSOCIATE_TAG(tag,T) template<> inline int yarp::sig::ImageOf<T>::pixelCode() const { return tag; }
*/


#ifdef GENERATE_YARP_EXPANSION_OF_IMAGE_SUBCLASSES_FOR_CPP
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
#else

  // insert expansion, so that there are doxygen classes for the user to see

template<> class ImageOf<PixelMono> : public Image { virtual int pixelSize() const { return sizeof(PixelMono); } virtual int pixelCode() const { return YARP_PIXEL_MONO; } inline PixelMono& pixel(int x, int y) const { return *((PixelMono *)(getPixelAddress(x,y))); } };
template<> class ImageOf<PixelRgb> : public Image { virtual int pixelSize() const { return sizeof(PixelRgb); } virtual int pixelCode() const { return YARP_PIXEL_RGB; } inline PixelRgb& pixel(int x, int y) const { return *((PixelRgb *)(getPixelAddress(x,y))); } };
template<> class ImageOf<PixelHsv> : public Image { virtual int pixelSize() const { return sizeof(PixelHsv); } virtual int pixelCode() const { return YARP_PIXEL_HSV; } inline PixelHsv& pixel(int x, int y) const { return *((PixelHsv *)(getPixelAddress(x,y))); } };
template<> class ImageOf<PixelBgr> : public Image { virtual int pixelSize() const { return sizeof(PixelBgr); } virtual int pixelCode() const { return YARP_PIXEL_BGR; } inline PixelBgr& pixel(int x, int y) const { return *((PixelBgr *)(getPixelAddress(x,y))); } };
template<> class ImageOf<PixelMonoSigned> : public Image { virtual int pixelSize() const { return sizeof(PixelMonoSigned); } virtual int pixelCode() const { return YARP_PIXEL_MONO_SIGNED; } inline PixelMonoSigned& pixel(int x, int y) const { return *((PixelMonoSigned *)(getPixelAddress(x,y))); } };
template<> class ImageOf<PixelRgbSigned> : public Image { virtual int pixelSize() const { return sizeof(PixelRgbSigned); } virtual int pixelCode() const { return YARP_PIXEL_RGB_SIGNED; } inline PixelRgbSigned& pixel(int x, int y) const { return *((PixelRgbSigned *)(getPixelAddress(x,y))); } };
template<> class ImageOf<PixelFloat> : public Image { virtual int pixelSize() const { return sizeof(PixelFloat); } virtual int pixelCode() const { return YARP_PIXEL_MONO_FLOAT; } inline PixelFloat& pixel(int x, int y) const { return *((PixelFloat *)(getPixelAddress(x,y))); } };
template<> class ImageOf<PixelRgbFloat> : public Image { virtual int pixelSize() const { return sizeof(PixelRgbFloat); } virtual int pixelCode() const { return YARP_PIXEL_RGB_FLOAT; } inline PixelRgbFloat& pixel(int x, int y) const { return *((PixelRgbFloat *)(getPixelAddress(x,y))); } };
template<> class ImageOf<PixelHsvFloat> : public Image { virtual int pixelSize() const { return sizeof(PixelHsvFloat); } virtual int pixelCode() const { return YARP_PIXEL_HSV_FLOAT; } inline PixelHsvFloat& pixel(int x, int y) const { return *((PixelHsvFloat *)(getPixelAddress(x,y))); } };
template<> class ImageOf<PixelInt> : public Image { virtual int pixelSize() const { return sizeof(PixelInt); } virtual int pixelCode() const { return YARP_PIXEL_INT; } inline PixelInt& pixel(int x, int y) const { return *((PixelInt *)(getPixelAddress(x,y))); } };

#endif


  }
}

#undef __YARPIMAGE_ASSOCIATE_TAG



#endif
