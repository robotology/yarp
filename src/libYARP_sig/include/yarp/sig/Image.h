// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_IMAGE_
#define _YARP2_IMAGE_

#include <yarp/os/Portable.h>

namespace yarp {
    /**
     * Signal processing.
     */
    namespace sig {
        class Image;
        class FlexImage;
        template <class T> class ImageOf;
    }
}


/**
 * Base class for storing images.
 * You actually want to use ImageOf or FlexImage.
 * This is a minimal class, designed to be as interoperable as possible
 * with other image classes in other libraries, particularly IPL-derived
 * libraries such as OpenCV.
 */
class yarp::sig::Image : public yarp::os::Portable {

public:

    /**
     * Default constructor.  
     * Creates an empty image.
     */
    Image();

    /**
     * Copy constructor.  
     * Clones the content of another image.
     * @param alt the image to clone
     */
    Image(const Image& alt);

    /**
     * Destructor.
     */
    virtual ~Image();

    /**
     * Assignment operator.
     * Clones the content of another image.
     * @param alt the image to clone
     */
    const Image& operator=(const Image& alt);


    /**
     * Copy operator.
     * Clones the content of another image.
     * @param alt the image to clone
     */
    bool copy(const Image& alt);


    /**
     * Gets width of image in pixels.
     * @return the width of the image in pixels (0 if no image present)
     */
    inline int width() const { return imgWidth; }

    /**
     * Gets height of image in pixels.
     * @return the height of the image in pixels (0 if no image present)
     */
    inline int height() const { return imgHeight; }

    /**
     * Gets pixel size in memory in bytes.
     * @return the size of the pixels stored in the image, in bytes
     */
    virtual int getPixelSize() const;

    /**
     * Gets pixel type identifier.
     * Images have an associated type identifier to
     * permit automatic casting between different image types.
     * @return the image type identifier
     */
    virtual int getPixelCode() const;

    /**
     * Size of the underlying image buffer rows.
     * @return size of the underlying image buffer rows in bytes.
     */
    inline int getRowSize() const { return imgRowSize; }


    /**
     * The size of a row is constrained to be a multiple of the "quantum".
     * @return size of the current quantum (0 means no constraint)
     */
    inline int getQuantum() const { return imgQuantum; }

    /**
     * Get address of a pixel in memory.
     * @param x x coordinate
     * @param y y coordinate
     * @return address of pixel in memory
     */
    inline unsigned char *getPixelAddress(int x, int y) const {
        return (unsigned char *)(data[y] + x*imgPixelSize);
    }

    /**
     * Check whether a coordinate lies within the image
     * @param x x coordinate
     * @param y y coordinate
     * @return true iff there is a pixel at the given coordinate
     */
    inline bool isPixel(int x, int y) const {
        return (x>=0 && y>=0 && x<imgWidth && y<imgHeight);
    }

    /**
     * Set all pixels to 0.
     */
    void zero();

    /**
     * Change the size of the image.
     * @param imgWidth the desired width (the number of possible x values)
     * @param imgHeight the desired width (the number of possible y values)
     */
    void resize(int imgWidth, int imgHeight);

    void setExternal(void *data, int imgWidth, int imgHeight);

    unsigned char *getRawImage() const;

    int getRawImageSize() const;

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


    /**
     * Read image from a connection.
     * return true iff image was read correctly
     */
    virtual bool read(ConnectionReader& connection);

    /**
     * Write image to a connection.
     * return true iff image was written correctly
     */
    virtual bool write(ConnectionWriter& connection);

    void setQuantum(int imgQuantum);

protected:

    void setPixelCode(int imgPixelCode);

    void setPixelSize(int imgPixelSize);


private:
    int imgWidth, imgHeight, imgPixelSize, imgRowSize, imgPixelCode, imgQuantum;
    char **data;
    void *implementation;

    void synchronize();
    void initialize();

    void copyPixels(const unsigned char *src, int id1, 
                    unsigned char *dest, int id2, int w, int h,
                    int imageSize, int quantum);
};


/**
 * Image class with user control of representation details.
 * Can be necessary when interfacing with other image types.
 */
class yarp::sig::FlexImage : public yarp::sig::Image {
public:

    void setPixelCode(int imgPixelCode) {
        Image::setPixelCode(imgPixelCode);
    }

    void setPixelSize(int imgPixelSize) {
        Image::setPixelSize(imgPixelSize);
    }

    void setQuantum(int imgQuantum) {
        Image::setQuantum(imgQuantum);
    }

private:
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

        /**
         * Monochrome pixel type.
         */
        typedef unsigned char PixelMono;

        /**
         * 32-bit integer pixel type.
         */
        typedef yarp::os::NetInt32 PixelInt;

        /**
         * Packed RGB pixel type.
         */
        struct PixelRgb
        { 
            unsigned char r,g,b; 
      
            PixelRgb() { r = g = b = 0; }
            PixelRgb(unsigned char n_r, unsigned char n_g, unsigned char n_b)
            { r = n_r;  g = n_g;  b = n_b; }
        } /** \cond */ PACKED_FOR_NET /** \endcond */;
    
        /**
         * Packed RGB pixel type, with pixels stored in reverse order.
         */
        struct PixelBgr
        { 
            unsigned char b,g,r; 
            PixelBgr() { b = g = r = 0; }
            PixelBgr(unsigned char n_r, unsigned char n_g, unsigned char n_b)
            { r = n_r;  g = n_g;  b = n_b; }
        } /** \cond */ PACKED_FOR_NET /** \endcond */;
    
        /**
         * Packed HSV (hue/saturation/value pixel type.
         */
        typedef struct { unsigned char h,s,v; } PixelHsv;

        /**
         * Signed byte pixel type.
         */
        typedef char PixelMonoSigned;

        /**
         * Signed, packed RGB pixel type.
         */
        typedef struct { char r,g,b; } PixelRgbSigned;

        /**
         * Floating point pixel type.
         */
        typedef float PixelFloat;

        /**
         * Floating point RGB pixel type.
         */
        typedef struct { float r,g,b; } PixelRgbFloat;

        /**
         * Floating point HSV pixel type.
         */
        typedef struct { float h,s,v; } PixelHsvFloat;

#include <yarp/os/end_pack_for_net.h>

    }
}


/**
 * Typed image class.
 *
 * This is a wrapper over YARPGenericImage providing type security for
 * pixel access.  "T" can be any of sig::PixelMono, sig::PixelRgb, 
 * sig::PixelHsv, sig::PixelBgr, sig::PixelMonoSigned, sig::PixelRgbSigned, 
 * sig::PixelFloat, sig::PixelRgbFloat, sig::PixelHsvFloat, 
 * sig::PixelInt.
 * If ImageOf::copy is called for two such images, a reasonable casting
 * operation will occur if the pixel types are different.
 *
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
