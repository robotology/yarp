/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_IMAGE_H
#define YARP_SIG_IMAGE_H

#include <yarp/conf/system.h>
#include <yarp/os/NetUint16.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Type.h>
#include <yarp/os/Vocab.h>
#include <yarp/sig/api.h>
#include <map>

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
        inline size_t PAD_BYTES (size_t len, size_t pad) {
            const size_t rem = len % pad;
            return (rem != 0) ? (pad - rem) : rem;
        }
    }
}

// the image types partially reflect the IPL image types.
// There must be a pixel type for every ImageType entry.
enum YarpVocabPixelTypesEnum
{
    VOCAB_PIXEL_INVALID = 0,
    VOCAB_PIXEL_MONO = yarp::os::createVocab32('m','o','n','o'),
    VOCAB_PIXEL_MONO16 = yarp::os::createVocab32('m','o','1','6'),
    VOCAB_PIXEL_RGB = yarp::os::createVocab32('r','g','b'),
    VOCAB_PIXEL_RGBA = yarp::os::createVocab32('r','g','b','a'),
    VOCAB_PIXEL_BGRA = yarp::os::createVocab32('b','g','r','a'),
    VOCAB_PIXEL_INT = yarp::os::createVocab32('i','n','t'),
    VOCAB_PIXEL_HSV = yarp::os::createVocab32('h','s','v'),
    VOCAB_PIXEL_BGR = yarp::os::createVocab32('b','g','r'),
    VOCAB_PIXEL_MONO_SIGNED = yarp::os::createVocab32('s','i','g','n'),
    VOCAB_PIXEL_RGB_SIGNED = yarp::os::createVocab32('r','g','b','-'),
    VOCAB_PIXEL_RGB_INT = yarp::os::createVocab32('r','g','b','i'),
    VOCAB_PIXEL_MONO_FLOAT = yarp::os::createVocab32('d','e','c'),
    VOCAB_PIXEL_RGB_FLOAT = yarp::os::createVocab32('r','g','b','.'),
    VOCAB_PIXEL_HSV_FLOAT = yarp::os::createVocab32('h','s','v','.'),
    VOCAB_PIXEL_ENCODING_BAYER_GRBG8 = yarp::os::createVocab32('g', 'r', 'b', 'g'),   //grbg8
    VOCAB_PIXEL_ENCODING_BAYER_GRBG16 = yarp::os::createVocab32('g', 'r', '1', '6'),  //grbg16
    VOCAB_PIXEL_ENCODING_BAYER_BGGR8 = yarp::os::createVocab32('b', 'g', 'g', 'r'),     //bggr8
    VOCAB_PIXEL_ENCODING_BAYER_BGGR16 = yarp::os::createVocab32('b', 'g', '1', '6'),  //bggr16
    VOCAB_PIXEL_ENCODING_BAYER_GBRG8 = yarp::os::createVocab32('g', 'b', 'r', 'g'),  //gbrg8
    VOCAB_PIXEL_ENCODING_BAYER_GBRG16 = yarp::os::createVocab32('g', 'b', '1', '6'),  //gbrg16
    VOCAB_PIXEL_ENCODING_BAYER_RGGB8 = yarp::os::createVocab32('r', 'g', 'g', 'b'),   //rggb8
    VOCAB_PIXEL_ENCODING_BAYER_RGGB16 = yarp::os::createVocab32('r', 'g', '1', '6'),  //rggb16
    VOCAB_PIXEL_YUV_420 = yarp::os::createVocab32('y','u','v','a'),
    VOCAB_PIXEL_YUV_444 = yarp::os::createVocab32('y','u','v','b'),
    VOCAB_PIXEL_YUV_422 = yarp::os::createVocab32('y','u','v','c'),
    VOCAB_PIXEL_YUV_411 = yarp::os::createVocab32('y','u','v','d')
};

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
     * @brief Move constructor.
     *
     * @param other the Image to be moved
     */
    Image(Image&& other) noexcept;

    /**
     * Assignment operator.
     * Clones the content of another image.
     * @param alt the image to clone
     */
    Image& operator=(const Image& alt);

    /**
     * @brief Move assignment operator.
     *
     * @param other the Image to be moved
     * @return this object
     */
    Image& operator=(Image &&other) noexcept;

    /**
     * Destructor.
     */
    ~Image() override;


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
    bool copy(const Image& alt, size_t w, size_t h);


    /**
     * move operator.
     * Moves the content of another image.
     * @param alt the image to move
     */
    bool move(Image&& alt) noexcept;


    /**
     * swap operator.
     * Swap the content with another image.
     * @param alt the image to swap with
     */
    bool swap(Image& alt);


    /**
     * Gets width of image in pixels.
     * @return the width of the image in pixels (0 if no image present)
     */
    inline size_t width() const { return imgWidth; }

    /**
     * Gets height of image in pixels.
     * @return the height of the image in pixels (0 if no image present)
     */
    inline size_t height() const { return imgHeight; }

    /**
     * Gets pixel size in memory in bytes.
     * @return the size of the pixels stored in the image, in bytes
     */
    virtual size_t getPixelSize() const;

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
    inline size_t getRowSize() const { return imgRowSize; }


    /**
     * The size of a row is constrained to be a multiple of the "quantum".
     * @return size of the current quantum (0 means no constraint)
     */
    inline size_t getQuantum() const { return imgQuantum; }

   /**
     * Returns the number of padding bytes.
     * @return number of bytes of the row padding.
     */
    inline size_t getPadding() const
    {
        const size_t ret=imgRowSize-imgWidth*imgPixelSize;
        return ret;
    }

    /**
     * Get the address of a the first byte of a row in memory.
     * @param r row number (starting from 0)
     * @return address of the r-th row
     */
    inline unsigned char *getRow(size_t r)
    {
        // should we check limits?
        return reinterpret_cast<unsigned char *>(data[r]);
    }

    /**
     * Get the address of a the first byte of a row in memory,
     * const versions.
     * @param r row number (starting from 0)
     * @return address of the r-th row
     */
    inline const unsigned char *getRow(size_t r) const
    {
        // should we check limits?
        return reinterpret_cast<const unsigned char *>(data[r]);
    }

    /**
     * Get address of a pixel in memory.
     * @param x x coordinate
     * @param y y coordinate
     * @return address of pixel in memory
     */
    inline unsigned char *getPixelAddress(size_t x, size_t y) const {
        return reinterpret_cast<unsigned char *>(data[y] + x*imgPixelSize);
    }

    /**
     * Check whether a coordinate lies within the image
     * @param x x coordinate
     * @param y y coordinate
     * @return true iff there is a pixel at the given coordinate
     */
    inline bool isPixel(size_t x, size_t y) const {
        return (x<imgWidth && y<imgHeight);
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
    void resize(size_t imgWidth, size_t imgHeight);

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
    void setExternal(const void *data, size_t imgWidth, size_t imgHeight);

    /**
    * Access to the internal image buffer.
    * @return pointer to the internal image buffer.
    */
    unsigned char *getRawImage() const;

    /**
    * Access to the internal buffer size information (this is how much memory has been allocated for the image).
    * @return size of the internal buffer in bytes including padding.
    */
    size_t getRawImageSize() const;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.2.0
    /**
     * Returns IPL/OpenCV view of image, if possible.
     * Not possible if the image is the wrong size, with no padding.
     * This method is currently not well documented.
     * @return pointer to an IplImage structure or nullptr
     */
    YARP_DEPRECATED_MSG("Use yarp::cv::toCvMat instead")
    void *getIplImage();

    /**
     * Returns IPL/OpenCV view of image, if possible.
     * Not possible if the image is the wrong size, with no padding.
     * This method is currently not well documented.
     * @return pointer to an IplImage structure or nullptr
     */
    YARP_DEPRECATED_MSG("Use yarp::cv::toCvMat instead")
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
    YARP_DEPRECATED_MSG("Use yarp::cv::fromCvMat instead")
    void wrapIplImage(void *iplImage);
#endif // YARP_NO_DEPRECATED

    //void wrapRawImage(void *buf, int imgWidth, int imgHeight);


    /**
     * Read image from a connection.
     * @return true iff image was read correctly
     */
    bool read(yarp::os::ConnectionReader& connection) override;

    /**
     * Write image to a connection.
     * @return true iff image was written correctly
     */
    bool write(yarp::os::ConnectionWriter& connection) const override;

    void setQuantum(size_t imgQuantum);

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
    void setTopIsLowIndex(bool flag);


    /**
     * Get an array of pointers to the rows of the image.
     * @return an array of pointers to the rows of the image.
     */
    char **getRowArray() {
        return data;
    }

    yarp::os::Type getReadType() const override {
        return yarp::os::Type::byName("yarp/image");
    }

protected:

    void setPixelCode(int imgPixelCode);

    //pixelCode and pixelsSize should be linked together consistently.
    //since setPixelCode set also the corresponding pixelSize setPixelSize should not be used at all except for
    //setting an arbitrary pixelSize with no corresponding pixel code (in that case the pixelCode will be set to -pixelSize).
    void setPixelSize(size_t imgPixelSize);


private:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING
    static const std::map<YarpVocabPixelTypesEnum, size_t> pixelCode2Size;
    size_t imgWidth, imgHeight, imgPixelSize, imgRowSize, imgQuantum;
    int imgPixelCode;
    bool topIsLow;

    char **data;
    void *implementation;

    void synchronize();
    void initialize();

    void copyPixels(const unsigned char *src, size_t id1,
                    unsigned char *dest, size_t id2, size_t w, size_t h,
                    size_t imageSize, size_t quantum1, size_t quantum2,
                    bool topIsLow1, bool topIsLow2);
};


/**
 * Image class with user control of representation details.
 * Can be necessary when interfacing with other image types.
 */
class YARP_sig_API yarp::sig::FlexImage : public yarp::sig::Image {
public:

    void setPixelCode(int imgPixelCode) {
        Image::setPixelCode(imgPixelCode);
    }


    void setPixelSize(size_t imgPixelSize) {
        Image::setPixelSize(imgPixelSize);
    //pixelCode and pixelsSize should be linked together consistently.
    //since setPixelCode set also the corresponding pixelSize setPixelSize should not be used at all except for
    //setting an arbitrary pixelSize with no corresponding pixel code (in that case the pixelCode will be set to -pixelSize).
    }

    void setQuantum(size_t imgQuantum) {
        Image::setQuantum(imgQuantum);
    }

private:
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
            unsigned char r{0};
            unsigned char g{0};
            unsigned char b{0};

            PixelRgb() = default;
            PixelRgb(unsigned char n_r,
                     unsigned char n_g,
                     unsigned char n_b) :
                    r(n_r),
                    g(n_g),
                    b(n_b)
            {
            }
        };
        YARP_END_PACK

        /**
         * Packed RGBA pixel type.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelRgba
        {
            PixelRgba() = default;
            PixelRgba(unsigned char n_r,
                      unsigned char n_g,
                      unsigned char n_b,
                      unsigned char n_a) :
                    r(n_r),
                    g(n_g),
                    b(n_b),
                    a(n_a)
            {
            }

            unsigned char r{0};
            unsigned char g{0};
            unsigned char b{0};
            unsigned char a{0};
        };
        YARP_END_PACK

        /**
         * Packed BGRA pixel type.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelBgra
        {
            unsigned char b{0};
            unsigned char g{0};
            unsigned char r{0};
            unsigned char a{0};

            PixelBgra() = default;
            PixelBgra(unsigned char n_r,
                      unsigned char n_g,
                      unsigned char n_b,
                      unsigned char n_a) :
                    b(n_b),
                    g(n_g),
                    r(n_r),
                    a(n_a)
            {
            }
        };
        YARP_END_PACK

        /**
         * Packed RGB pixel type, with pixels stored in reverse order.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelBgr
        {
            unsigned char b{0};
            unsigned char g{0};
            unsigned char r{0};

            PixelBgr() = default;
            PixelBgr(unsigned char n_r, unsigned char n_g, unsigned char n_b) :
                    b(n_b),
                    g(n_g),
                    r(n_r)
            {
            }
        };
        YARP_END_PACK

        /**
         * Packed HSV (hue/saturation/value pixel type.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelHsv
        {
            unsigned char h{0};
            unsigned char s{0};
            unsigned char v{0};
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
        struct YARP_sig_API PixelRgbSigned
        {
            char r{0};
            char g{0};
            char b{0};
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
        struct YARP_sig_API PixelRgbFloat
        {
            float r{0.0F};
            float g{0.0F};
            float b{0.0F};

            PixelRgbFloat() = default;
            PixelRgbFloat(float n_r,
                          float n_g,
                          float n_b) :
                    r(n_r),
                    g(n_g),
                    b(n_b)
            {
            }
        };
        YARP_END_PACK

        /**
         * Integer RGB pixel type.
         */
        YARP_BEGIN_PACK
        struct YARP_sig_API PixelRgbInt
        {
            yarp::os::NetInt32 r{0};
            yarp::os::NetInt32 g{0};
            yarp::os::NetInt32 b{0};

            PixelRgbInt() = default;
            PixelRgbInt(int n_r,
                        int n_g,
                        int n_b) :
                    r(n_r),
                    g(n_g),
                    b(n_b)
            {
            }
        };
        YARP_END_PACK

        /**
         * Floating point HSV pixel type.
         */
        YARP_BEGIN_PACK
        struct PixelHsvFloat
        {
            float h{0.0F};
            float s{0.0F};
            float v{0.0F};
        };
        YARP_END_PACK
    }
}


/**
 * Typed image class.
 *
 * This is a wrapper over yarp::sig::Image providing type security for
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
    ImageOf() : Image(),
        nullPixel()
    {
        setPixelCode(getPixelCode());
    }

    size_t getPixelSize() const override {
        return sizeof(T);
    }

    int getPixelCode() const override;

    inline T& pixel(size_t x, size_t y) {
        return *(reinterpret_cast<T*>(getPixelAddress(x,y)));
    }

    inline T& pixel(size_t x, size_t y) const {
        return *(reinterpret_cast<T*>(getPixelAddress(x,y)));
    }

    inline const T& operator()(size_t x, size_t y) const {
        return pixel(x,y);
    }

    inline T& operator()(size_t x, size_t y) {
        return pixel(x,y);
    }

    inline T& safePixel(size_t x, size_t y) {
        if (!isPixel(x,y)) { return nullPixel; }
        return *(reinterpret_cast<T*>(getPixelAddress(x,y)));
    }

    inline const T& safePixel(size_t x, size_t y) const {
        if (!isPixel(x,y)) { return nullPixel; }
        return *(reinterpret_cast<T*>(getPixelAddress(x,y)));
    }
};

namespace yarp {
namespace sig {

template<>
inline int ImageOf<yarp::sig::PixelMono>::getPixelCode() const {
    return VOCAB_PIXEL_MONO;
}

template<>
inline int ImageOf<yarp::sig::PixelMono16>::getPixelCode() const {
    return VOCAB_PIXEL_MONO16;
}

template<>
inline int ImageOf<yarp::sig::PixelRgb>::getPixelCode() const {
    return VOCAB_PIXEL_RGB;
}

template<>
inline int ImageOf<yarp::sig::PixelRgba>::getPixelCode() const {
    return VOCAB_PIXEL_RGBA;
}

template<>
inline int ImageOf<yarp::sig::PixelHsv>::getPixelCode() const {
    return VOCAB_PIXEL_HSV;
}

template<>
inline int ImageOf<yarp::sig::PixelBgr>::getPixelCode() const {
    return VOCAB_PIXEL_BGR;
}

template<>
inline int ImageOf<yarp::sig::PixelBgra>::getPixelCode() const {
    return VOCAB_PIXEL_BGRA;
}

template<>
inline int ImageOf<yarp::sig::PixelMonoSigned>::getPixelCode() const {
    return VOCAB_PIXEL_MONO_SIGNED;
}

template<>
inline int ImageOf<yarp::sig::PixelRgbSigned>::getPixelCode() const {
    return VOCAB_PIXEL_RGB_SIGNED;
}

template<>
inline int ImageOf<yarp::sig::PixelFloat>::getPixelCode() const {
    return VOCAB_PIXEL_MONO_FLOAT;
}

template<>
inline int ImageOf<yarp::sig::PixelRgbFloat>::getPixelCode() const {
    return VOCAB_PIXEL_RGB_FLOAT;
}

template<>
inline int ImageOf<yarp::sig::PixelRgbInt>::getPixelCode() const {
    return VOCAB_PIXEL_RGB_INT;
}

template<>
inline int ImageOf<yarp::sig::PixelHsvFloat>::getPixelCode() const {
    return VOCAB_PIXEL_HSV_FLOAT;
}

template<>
inline int ImageOf<yarp::sig::PixelInt>::getPixelCode() const {
    return VOCAB_PIXEL_INT;
}

template<typename T>
inline int ImageOf<T>::getPixelCode() const {
    return -(static_cast<int>(sizeof(T)));
}

} // namespace sig
} // namespace yarp

#endif // YARP_SIG_IMAGE_H
