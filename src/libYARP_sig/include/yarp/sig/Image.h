// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_IMAGE_
#define _YARP2_IMAGE_

#include <yarp/conf/system.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/NetUint16.h>
#include <yarp/sig/api.h>

namespace yarp {
    /**
     * Signal processing.
     */
    namespace sig {
        class Image;
        class FlexImage;
        template <class T> class ImageOf;

        /**
         * computes the padding of YARP images.
         * @param len is the row length in bytes
         * @param pad is the desired padding (e.g. 8 bytes)
         * @return the number of extra bytes to add at the end of the image row
         */
        inline int PAD_BYTES (int len, int pad) {
            const int rem = len % pad;
            return (rem != 0) ? (pad - rem) : rem;
        }
    }
}


/**
 * \ingroup sig_class
 *
 * Base class for storing images.
 * You actually want to use ImageOf or FlexImage.
 * This is a minimal class, designed to be as interoperable as possible
 * with other image classes in other libraries, particularly IPL-derived
 * libraries such as OpenCV.
 */
class YARP_sig_API yarp::sig::Image : public yarp::os::Portable {

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
     * Scaled copy.
     * Clones the content of another image, and resizes in a fast but
     * low-quality way.
     * @param alt the image to copy
     * @param w target width for image
     * @param h target height for image
     */
    bool copy(const Image& alt, int w, int h);


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
     * Returns the number of padding bytes.
     * @return number of bytes of the row padding.
     */
    inline int getPadding() const
    {
        const int ret=imgRowSize-imgWidth*imgPixelSize;
        return ret;
    }

    /**
     * Get the address of a the first byte of a row in memory.
     * @param r row number (starting from 0)
     * @return address of the r-th row
     */
    inline unsigned char *getRow(int r)
    {
        // should we check limits?
        return (unsigned char *)(data[r]);
    }

    /**
     * Get the address of a the first byte of a row in memory,
     * const versions.
     * @param r row number (starting from 0)
     * @return address of the r-th row
     */
    inline const unsigned char *getRow(int r) const
    {
        // should we check limits?
        return (const unsigned char *)(data[r]);
    }

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
     * Reallocate an image to be of a desired size, throwing away its
     * current contents.  If the desired size is the same as the current
     * size, then no reallocation is done.  But the resulting image
     * should always be assumed to have undefined content.  To rescale
     * an image, maintaining its content, see the Image::copy methods.
     * @param imgWidth the desired width (the number of possible x values)
     * @param imgHeight the desired height (the number of possible y values)
     */
    void resize(int imgWidth, int imgHeight);

    /**
     * Reallocate the size of the image to match another, throwing
     * away the actual content of the image.
     * @param alt the image whose size we should match.
     */
    void resize(const Image& alt) {
        resize(alt.width(),alt.height());
    }

    /**
     * Use this to wrap an external image.
     * Make sure to that pixel type and padding quantum are
     * synchronized (you can set these in the FlexImage class).
     */
    void setExternal(void *data, int imgWidth, int imgHeight);

    /**
    * Access to the internal image buffer.
    * @return pointer to the internal image buffer.
    */
    unsigned char *getRawImage() const;

    /**
    * Access to the internal buffer size information (this is how much memory has been allocated for the image).
    * @return size of the internal buffer in bytes including padding.
    */
    int getRawImageSize() const;

    /**
     * Returns IPL/OpenCV view of image, if possible.
     * Not possible if the image is the wrong size, with no padding.
     * This method is currently not well documented.
     * @return pointer to an IplImage structure
     */
    void *getIplImage();

    /**
     * Returns IPL/OpenCV view of image, if possible.
     * Not possible if the image is the wrong size, with no padding.
     * This method is currently not well documented.
     * @return pointer to an IplImage structure
     */
    const void *getIplImage() const;

    /**
     * Act as a wrapper around an IPL/OpenCV image.  The wrapped
     * image needs to exist for the rest of the lifetime of
     * this oboject.  Be careful if you use this method on objects
     * read from or written to a BufferedPort, since the lifetime
     * of such objects can be longer than you expect (see the
     * documentation for yarp::os::BufferedPort::read,
     * yarp::os::BufferedPort::prepare, and yarp::os::BufferedPort::write).
     *
     * @param iplImage pointer to an IplImage structure
     */
    void wrapIplImage(void *iplImage);

    //void wrapRawImage(void *buf, int imgWidth, int imgHeight);


    /**
     * Read image from a connection.
     * @return true iff image was read correctly
     */
    virtual bool read(ConnectionReader& connection);

    /**
     * Write image to a connection.
     * @return true iff image was written correctly
     */
    virtual bool write(ConnectionWriter& connection);

    void setQuantum(int imgQuantum);

    /**
     * @return true if image has origin at top left (default); in other
     * words when the y index is low, we are near the top of the image.
     */
    bool topIsLowIndex() const {
        return topIsLow;
    }

    /**
     * control whether image has origin at top left (default) or bottom
     * left.
     *
     * @param flag true if image has origin at top left (default),
     * false if image has origin at bottom left.
     *
     */
    void setTopIsLowIndex(bool flag) {
        topIsLow = flag;
    }


    /**
     * Get an array of pointers to the rows of the image.
     * @return an array of pointers to the rows of the image.
     */
    char **getRowArray() {
        return data;
    }

    virtual yarp::os::Type getReadType() {
        return yarp::os::Type::byName("yarp/image");
    }

protected:

    void setPixelCode(int imgPixelCode);

    void setPixelSize(int imgPixelSize);


private:

    int imgWidth, imgHeight, imgPixelSize, imgRowSize, imgPixelCode, imgQuantum;
    bool topIsLow;

    char **data;
    void *implementation;

    void synchronize();
    void initialize();

    void copyPixels(const unsigned char *src, int id1,
                    unsigned char *dest, int id2, int w, int h,
                    int imageSize, int quantum1, int quantum2,
                    bool topIsLow1, bool topIsLow2);
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



// the image types partially reflect the IPL image types.
// There must be a pixel type for every ImageType entry.
enum YarpVocabPixelTypesEnum
    {
        VOCAB_PIXEL_INVALID = 0,
        VOCAB_PIXEL_MONO = VOCAB4('m','o','n','o'),
        VOCAB_PIXEL_MONO16 = VOCAB4('m','o','1','6'),
        VOCAB_PIXEL_RGB = VOCAB3('r','g','b'),
        VOCAB_PIXEL_RGBA = VOCAB4('r','g','b','a'),
        VOCAB_PIXEL_BGRA = VOCAB4(98/*'b'*/,'g','r','a'), /* SWIG BUG */
        VOCAB_PIXEL_INT = VOCAB3('i','n','t'),
        VOCAB_PIXEL_HSV = VOCAB3('h','s','v'),
        VOCAB_PIXEL_BGR = VOCAB3(98/*'b'*/,'g','r'), /* SWIG BUG */
        VOCAB_PIXEL_MONO_SIGNED = VOCAB4('s','i','g','n'),
        VOCAB_PIXEL_RGB_SIGNED = VOCAB4('r','g','b','-'),
        VOCAB_PIXEL_RGB_INT = VOCAB4('r','g','b','i'),
        VOCAB_PIXEL_MONO_FLOAT = VOCAB3('d','e','c'),
        VOCAB_PIXEL_RGB_FLOAT = VOCAB4('r','g','b','.'),
        VOCAB_PIXEL_HSV_FLOAT = VOCAB4('h','s','v','.')
    };




#include <yarp/os/NetInt32.h>

namespace yarp {
    namespace sig {

        /**
         * Monochrome pixel type.
         */
        typedef unsigned char PixelMono;

        /**
         * 16-bit monochrome pixel type.
         */
        typedef yarp::os::NetUint16 PixelMono16;

        /**
         * 32-bit integer pixel type.
         */
        typedef yarp::os::NetInt32 PixelInt;

        /**
         * Packed RGB pixel type.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelRgb
        {
            unsigned char r,g,b;

            PixelRgb() { r = g = b = 0; }
            PixelRgb(unsigned char n_r, unsigned char n_g, unsigned char n_b)
            { r = n_r;  g = n_g;  b = n_b; }
        };
        YARP_END_PACK

        /**
         * Packed RGBA pixel type.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelRgba
        {
            unsigned char r,g,b,a;

            PixelRgba() { r = g = b = a = 0; }
            PixelRgba(unsigned char n_r, unsigned char n_g,
                      unsigned char n_b, unsigned char n_a)
            { r = n_r;  g = n_g;  b = n_b; a = n_a; }
        };
        YARP_END_PACK

        /**
         * Packed BGRA pixel type.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelBgra
        {
            unsigned char b,g,r,a;

            PixelBgra() { r = g = b = a = 0; }
            PixelBgra(unsigned char n_r, unsigned char n_g,
                      unsigned char n_b, unsigned char n_a)
            { r = n_r;  g = n_g;  b = n_b; a = n_a; }
        };
        YARP_END_PACK

        /**
         * Packed RGB pixel type, with pixels stored in reverse order.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelBgr
        {
            unsigned char b,g,r;
            PixelBgr() { b = g = r = 0; }
            PixelBgr(unsigned char n_r, unsigned char n_g, unsigned char n_b)
            { r = n_r;  g = n_g;  b = n_b; }
        };
        YARP_END_PACK

        /**
         * Packed HSV (hue/saturation/value pixel type.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelHsv {
            unsigned char h,s,v;
        };
        YARP_END_PACK

        /**
         * Signed byte pixel type.
         */
        typedef char PixelMonoSigned;

        /**
         * Signed, packed RGB pixel type.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelRgbSigned {
            char r,g,b;
        };
        YARP_END_PACK

        /**
         * Floating point pixel type.
         */
        typedef float PixelFloat;

        /**
         * Floating point RGB pixel type.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelRgbFloat {
            float r,g,b;
            PixelRgbFloat() { r = g = b = 0; }
            PixelRgbFloat(float n_r, float n_g, float n_b)
            { r = n_r;  g = n_g;  b = n_b; }
        };
        YARP_END_PACK

        /**
         * Integer RGB pixel type.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelRgbInt {
            yarp::os::NetInt32 r,g,b;
            PixelRgbInt() { r = g = b = 0; }
            PixelRgbInt(int n_r, int n_g, int n_b) {
                r = n_r; g = n_g; b = n_b;
            }
        };
        YARP_END_PACK

        /**
         * Floating point HSV pixel type.
         */
        YARP_BEGIN_PACK
        struct PixelHsvFloat {
            float h,s,v;
        };
        YARP_END_PACK

    }
}


/**
 * Typed image class.
 *
 * This is a wrapper over YARPGenericImage providing type security for
 * pixel access.  "T" can be any of sig::PixelMono, sig::PixelMono16,
 * sig::PixelRgb, sig::PixelHsv, sig::PixelBgr, sig::PixelMonoSigned,
 * sig::PixelRgbSigned, sig::PixelFloat, sig::PixelRgbFloat,
 * sig::PixelHsvFloat, sig::PixelInt.
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
        return -((int) sizeof(T));
    }

    inline T& pixel(int x, int y) {
        return *((T *)(getPixelAddress(x,y)));
    }

    inline T& pixel(int x, int y) const {
        return *((T *)(getPixelAddress(x,y)));
    }

    inline const T& operator()(int x, int y) const {
        return pixel(x,y);
    }

    inline T& operator()(int x, int y) {
        return pixel(x,y);
    }

    inline T& safePixel(int x, int y) {
        if (!isPixel(x,y)) { return nullPixel; }
        return *((T *)(getPixelAddress(x,y)));
    }

    inline const T& safePixel(int x, int y) const {
        if (!isPixel(x,y)) { return nullPixel; }
        return *((T *)(getPixelAddress(x,y)));
    }
};

namespace yarp {
    namespace sig {

#define __YARPIMAGE_ASSOCIATE_TAG(tag,T) \
template<> \
class YARP_sig_API ImageOf<T> : public Image \
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
  inline const T& pixel(int x, int y) const { \
    return *((T *)(getPixelAddress(x,y))); \
  } \
\
  inline T& operator()(int x, int y) { \
    return pixel(x,y); \
  } \
\
  inline const T& operator()(int x, int y) const { \
    return pixel(x,y); \
  } \
\
  inline T& safePixel(int x, int y) { \
    if (!isPixel(x,y)) { return nullPixel; } \
    return *((T *)(getPixelAddress(x,y))); \
  } \
\
  inline const T& safePixel(int x, int y) const { \
    if (!isPixel(x,y)) { return nullPixel; } \
    return *((T *)(getPixelAddress(x,y))); \
  } \
};


            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_MONO,PixelMono)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_MONO16,PixelMono16)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_RGB,PixelRgb)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_RGBA,PixelRgba)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_HSV,PixelHsv)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_BGR,PixelBgr)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_BGRA,PixelBgra)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_MONO_SIGNED,PixelMonoSigned)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_RGB_SIGNED,PixelRgbSigned)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_MONO_FLOAT,PixelFloat)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_RGB_FLOAT,PixelRgbFloat)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_RGB_INT,PixelRgbInt)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_HSV_FLOAT,PixelHsvFloat)
            __YARPIMAGE_ASSOCIATE_TAG(VOCAB_PIXEL_INT,PixelInt)

            }
}

#undef __YARPIMAGE_ASSOCIATE_TAG


#ifndef YARP_IMAGE_HEADER_CONTROL
#define YARP_IMAGE_HEADER_CONTROL
#endif

#endif
