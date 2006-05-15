// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/Logger.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/IplImage.h>

using namespace yarp;
using namespace yarp::sig;


// Default copy mechanism
template <class T1, class T2>
static inline void CopyPixel(const T1 *src, T2 *dest)
{
	*dest = *src;
}

typedef PixelMono Def_YARP_PIXEL_MONO;
typedef PixelRgb Def_YARP_PIXEL_RGB;
typedef PixelHsv Def_YARP_PIXEL_HSV;
typedef PixelBgr Def_YARP_PIXEL_BGR;
typedef PixelMonoSigned Def_YARP_PIXEL_MONO_SIGNED;
typedef PixelRgbSigned Def_YARP_PIXEL_RGB_SIGNED;
typedef PixelFloat Def_YARP_PIXEL_MONO_FLOAT;
typedef PixelRgbFloat Def_YARP_PIXEL_RGB_FLOAT;
typedef PixelHsvFloat Def_YARP_PIXEL_HSV_FLOAT;
typedef PixelInt Def_YARP_PIXEL_INT;

#define VALID_PIXEL(x) ((x>255)?255:((x<0)?0:x))
#define SPECIAL_COPY_BEGIN static void YARPDummyCopyPixel() {
#define SPECIAL_COPY(id1,id2) } static inline void CopyPixel(const Def_##id1 *src, Def_##id2 *dest) {
#define SPECIAL_COPY_END }

static int implemented_yet = 1;

SPECIAL_COPY_BEGIN

SPECIAL_COPY(YARP_PIXEL_MONO,YARP_PIXEL_RGB)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_MONO,YARP_PIXEL_BGR)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_MONO,YARP_PIXEL_HSV)
    dest->v = *src;
dest->h = dest->s = 0;
SPECIAL_COPY(YARP_PIXEL_MONO,YARP_PIXEL_RGB_SIGNED)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_MONO,YARP_PIXEL_RGB_FLOAT)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_MONO,YARP_PIXEL_HSV_FLOAT)
    dest->v = *src;
dest->h = dest->s = 0;
SPECIAL_COPY(YARP_PIXEL_MONO,YARP_PIXEL_MONO_SIGNED)
    *dest = *src >> 1;
SPECIAL_COPY(YARP_PIXEL_MONO,YARP_PIXEL_INT)
    *dest = *src;
SPECIAL_COPY(YARP_PIXEL_MONO,YARP_PIXEL_MONO_FLOAT)
    *dest = *src;

SPECIAL_COPY(YARP_PIXEL_RGB,YARP_PIXEL_MONO)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_RGB,YARP_PIXEL_INT)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_RGB,YARP_PIXEL_HSV)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_RGB,YARP_PIXEL_MONO_SIGNED)
    *dest = (char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_RGB,YARP_PIXEL_RGB_SIGNED)
    dest->r = src->r; dest->g = src->g; dest->b = src->b;
SPECIAL_COPY(YARP_PIXEL_RGB,YARP_PIXEL_MONO_FLOAT)
    *dest = ((src->r + src->g + src->b)/3.0f);
SPECIAL_COPY(YARP_PIXEL_RGB,YARP_PIXEL_RGB_FLOAT)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(YARP_PIXEL_RGB,YARP_PIXEL_BGR)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(YARP_PIXEL_RGB,YARP_PIXEL_HSV_FLOAT)
    ACE_ASSERT(implemented_yet == 0);


SPECIAL_COPY(YARP_PIXEL_HSV,YARP_PIXEL_MONO)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV,YARP_PIXEL_RGB)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV,YARP_PIXEL_BGR)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV,YARP_PIXEL_MONO_SIGNED)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV,YARP_PIXEL_RGB_SIGNED)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV,YARP_PIXEL_MONO_FLOAT)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV,YARP_PIXEL_RGB_FLOAT)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV,YARP_PIXEL_HSV_FLOAT)
    ACE_ASSERT(implemented_yet == 0);


SPECIAL_COPY(YARP_PIXEL_BGR,YARP_PIXEL_MONO)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_BGR,YARP_PIXEL_INT)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_BGR,YARP_PIXEL_HSV)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_BGR,YARP_PIXEL_MONO_SIGNED)
    *dest = (char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_BGR,YARP_PIXEL_RGB_SIGNED)
    dest->r = src->r; dest->g = src->g; dest->b = src->b;
SPECIAL_COPY(YARP_PIXEL_BGR,YARP_PIXEL_MONO_FLOAT)
    *dest = ((src->r + src->g + src->b)/3.0f);
SPECIAL_COPY(YARP_PIXEL_BGR,YARP_PIXEL_RGB_FLOAT)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(YARP_PIXEL_BGR,YARP_PIXEL_RGB)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(YARP_PIXEL_BGR,YARP_PIXEL_HSV_FLOAT)
    ACE_ASSERT(implemented_yet == 0);


SPECIAL_COPY(YARP_PIXEL_MONO_SIGNED,YARP_PIXEL_RGB)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_MONO_SIGNED,YARP_PIXEL_BGR)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_MONO_SIGNED,YARP_PIXEL_HSV)
    dest->v = *src;
dest->h = dest->s = 0;
SPECIAL_COPY(YARP_PIXEL_MONO_SIGNED,YARP_PIXEL_RGB_SIGNED)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_MONO_SIGNED,YARP_PIXEL_RGB_FLOAT)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_MONO_SIGNED,YARP_PIXEL_HSV_FLOAT)
    dest->v = *src;
dest->h = dest->s = 0;
SPECIAL_COPY(YARP_PIXEL_MONO_SIGNED,YARP_PIXEL_MONO)
    *dest = *src + 128;
SPECIAL_COPY(YARP_PIXEL_MONO_SIGNED,YARP_PIXEL_INT)
    *dest = *src;

SPECIAL_COPY(YARP_PIXEL_RGB_SIGNED,YARP_PIXEL_MONO)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_RGB_SIGNED,YARP_PIXEL_INT)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_RGB_SIGNED,YARP_PIXEL_HSV)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_RGB_SIGNED,YARP_PIXEL_MONO_SIGNED)
    *dest = (char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_RGB_SIGNED,YARP_PIXEL_RGB)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(YARP_PIXEL_RGB_SIGNED,YARP_PIXEL_BGR)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(YARP_PIXEL_RGB_SIGNED,YARP_PIXEL_MONO_FLOAT)
    *dest = ((src->r + src->g + src->b)/3.0f);
SPECIAL_COPY(YARP_PIXEL_RGB_SIGNED,YARP_PIXEL_RGB_FLOAT)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(YARP_PIXEL_RGB_SIGNED,YARP_PIXEL_HSV_FLOAT)
    ACE_ASSERT(implemented_yet == 0);

SPECIAL_COPY(YARP_PIXEL_MONO_FLOAT,YARP_PIXEL_MONO)
    *dest = (unsigned char)*src;
SPECIAL_COPY(YARP_PIXEL_MONO_FLOAT,YARP_PIXEL_INT)
    *dest = (unsigned char)*src;
SPECIAL_COPY(YARP_PIXEL_MONO_FLOAT,YARP_PIXEL_MONO_SIGNED)
    *dest = (char)*src;
SPECIAL_COPY(YARP_PIXEL_MONO_FLOAT,YARP_PIXEL_RGB)
    dest->r = dest->g = dest->b = (unsigned char)(*src);
SPECIAL_COPY(YARP_PIXEL_MONO_FLOAT,YARP_PIXEL_BGR)
    dest->r = dest->g = dest->b = (unsigned char)(*src);
SPECIAL_COPY(YARP_PIXEL_MONO_FLOAT,YARP_PIXEL_HSV)
    dest->v = (unsigned char)*src;
dest->h = dest->s = 0;
SPECIAL_COPY(YARP_PIXEL_MONO_FLOAT,YARP_PIXEL_RGB_SIGNED)
    dest->r = dest->g = dest->b = (signed char) *src;
SPECIAL_COPY(YARP_PIXEL_MONO_FLOAT,YARP_PIXEL_RGB_FLOAT)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_MONO_FLOAT,YARP_PIXEL_HSV_FLOAT)
    dest->v = *src;
dest->h = dest->s = 0;

SPECIAL_COPY(YARP_PIXEL_RGB_FLOAT,YARP_PIXEL_MONO)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_RGB_FLOAT,YARP_PIXEL_INT)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_RGB_FLOAT,YARP_PIXEL_HSV)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_RGB_FLOAT,YARP_PIXEL_MONO_SIGNED)
    *dest = (char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_RGB_FLOAT,YARP_PIXEL_RGB)
    dest->r = (unsigned char) src->r;
dest->g = (unsigned char) src->g;
dest->b = (unsigned char) src->b;
SPECIAL_COPY(YARP_PIXEL_RGB_FLOAT,YARP_PIXEL_BGR)
    dest->r = (unsigned char) src->r;
dest->g = (unsigned char) src->g;
dest->b = (unsigned char) src->b;
SPECIAL_COPY(YARP_PIXEL_RGB_FLOAT,YARP_PIXEL_MONO_FLOAT)
    *dest = ((src->r + src->g + src->b)/3);
SPECIAL_COPY(YARP_PIXEL_RGB_FLOAT,YARP_PIXEL_RGB_SIGNED)
    dest->r = (signed char) src->r;
dest->g = (signed char) src->g;
dest->b = (signed char) src->b;
SPECIAL_COPY(YARP_PIXEL_RGB_FLOAT,YARP_PIXEL_HSV_FLOAT)
    ACE_ASSERT(implemented_yet == 0);

SPECIAL_COPY(YARP_PIXEL_HSV_FLOAT,YARP_PIXEL_MONO)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV_FLOAT,YARP_PIXEL_RGB)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV_FLOAT,YARP_PIXEL_BGR)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV_FLOAT,YARP_PIXEL_MONO_SIGNED)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV_FLOAT,YARP_PIXEL_RGB_SIGNED)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV_FLOAT,YARP_PIXEL_MONO_FLOAT)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV_FLOAT,YARP_PIXEL_RGB_FLOAT)
    ACE_ASSERT(implemented_yet == 0);
SPECIAL_COPY(YARP_PIXEL_HSV_FLOAT,YARP_PIXEL_HSV)
    ACE_ASSERT(implemented_yet == 0);

SPECIAL_COPY(YARP_PIXEL_INT,YARP_PIXEL_RGB)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_INT,YARP_PIXEL_BGR)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_INT,YARP_PIXEL_HSV)
    dest->v = *src;
dest->h = dest->s = 0;
SPECIAL_COPY(YARP_PIXEL_INT,YARP_PIXEL_RGB_SIGNED)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_INT,YARP_PIXEL_RGB_FLOAT)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(YARP_PIXEL_INT,YARP_PIXEL_HSV_FLOAT)
    dest->v = *src;
dest->h = dest->s = 0;
SPECIAL_COPY(YARP_PIXEL_INT,YARP_PIXEL_MONO_SIGNED)
    *dest = *src >> 1;
SPECIAL_COPY(YARP_PIXEL_INT,YARP_PIXEL_MONO)
    *dest = *src;

SPECIAL_COPY_END

static inline int PAD_BYTES (int len, int pad)
{
	const int rem = len % pad;
	return (rem != 0) ? (pad - rem) : rem;
}

///
///
template <class T1, class T2>
static void CopyPixels(const T1 *src, T2 *dest, int w, int h)
{
	const int p1 = PAD_BYTES (w * sizeof(T1), YARP_IMAGE_ALIGN);
	const int p2 = PAD_BYTES (w * sizeof(T2), YARP_IMAGE_ALIGN);

	for (int i=0; i<h; i++)
        {
            for (int j = 0; j < w; j++)
                {
                    CopyPixel(src,dest);
                    src++;
                    dest++;
                }

            src = (const T1*)(((char *)src) + p1);
            dest = (T2*)(((char *)dest) + p2);
        }
}


#define HASH(id1,id2) ((id1)*256+(id2))
#define HANDLE_CASE(len,x1,T1,x2,T2) CopyPixels((T1*)x1,(T2*)x2,w,h);
#define MAKE_CASE(id1,id2) case HASH(id1,id2): HANDLE_CASE(len,src,Def_##id1,dest,Def_##id2); break;
#define MAKE_2CASE(id1,id2) MAKE_CASE(id1,id2); MAKE_CASE(id2,id1);

// More elegant ways to do this, but needs to be efficient at pixel level
void Image::copyPixels(const char *src, int id1, 
                       char *dest, int id2, int w, int h,
                       int imageSize, int quantum)
{
    if (id1==id2) {
        memcpy(dest,src,imageSize);
        return;
    }


    switch(HASH(id1,id2))
        {
            // Macros rely on len, x1, x2 variable names

            MAKE_2CASE(YARP_PIXEL_MONO,YARP_PIXEL_RGB);
            MAKE_2CASE(YARP_PIXEL_MONO,YARP_PIXEL_HSV);
            MAKE_2CASE(YARP_PIXEL_MONO,YARP_PIXEL_BGR);
            MAKE_2CASE(YARP_PIXEL_MONO,YARP_PIXEL_MONO_SIGNED);
            MAKE_2CASE(YARP_PIXEL_MONO,YARP_PIXEL_RGB_SIGNED);
            MAKE_2CASE(YARP_PIXEL_MONO,YARP_PIXEL_MONO_FLOAT);
            MAKE_2CASE(YARP_PIXEL_MONO,YARP_PIXEL_RGB_FLOAT);
            MAKE_2CASE(YARP_PIXEL_MONO,YARP_PIXEL_HSV_FLOAT);

            MAKE_2CASE(YARP_PIXEL_RGB,YARP_PIXEL_HSV);
            MAKE_2CASE(YARP_PIXEL_RGB,YARP_PIXEL_BGR);
            MAKE_2CASE(YARP_PIXEL_RGB,YARP_PIXEL_MONO_SIGNED);
            MAKE_2CASE(YARP_PIXEL_RGB,YARP_PIXEL_RGB_SIGNED);
            MAKE_2CASE(YARP_PIXEL_RGB,YARP_PIXEL_MONO_FLOAT);
            MAKE_2CASE(YARP_PIXEL_RGB,YARP_PIXEL_RGB_FLOAT);
            MAKE_2CASE(YARP_PIXEL_RGB,YARP_PIXEL_HSV_FLOAT);

            MAKE_2CASE(YARP_PIXEL_HSV,YARP_PIXEL_BGR);
            MAKE_2CASE(YARP_PIXEL_HSV,YARP_PIXEL_MONO_SIGNED);
            MAKE_2CASE(YARP_PIXEL_HSV,YARP_PIXEL_RGB_SIGNED);
            MAKE_2CASE(YARP_PIXEL_HSV,YARP_PIXEL_MONO_FLOAT);
            MAKE_2CASE(YARP_PIXEL_HSV,YARP_PIXEL_RGB_FLOAT);
            MAKE_2CASE(YARP_PIXEL_HSV,YARP_PIXEL_HSV_FLOAT);

            MAKE_2CASE(YARP_PIXEL_BGR,YARP_PIXEL_MONO_SIGNED);
            MAKE_2CASE(YARP_PIXEL_BGR,YARP_PIXEL_RGB_SIGNED);
            MAKE_2CASE(YARP_PIXEL_BGR,YARP_PIXEL_MONO_FLOAT);
            MAKE_2CASE(YARP_PIXEL_BGR,YARP_PIXEL_RGB_FLOAT);
            MAKE_2CASE(YARP_PIXEL_BGR,YARP_PIXEL_HSV_FLOAT);

            MAKE_2CASE(YARP_PIXEL_MONO_SIGNED,YARP_PIXEL_RGB_SIGNED);
            MAKE_2CASE(YARP_PIXEL_MONO_SIGNED,YARP_PIXEL_MONO_FLOAT);
            MAKE_2CASE(YARP_PIXEL_MONO_SIGNED,YARP_PIXEL_RGB_FLOAT);
            MAKE_2CASE(YARP_PIXEL_MONO_SIGNED,YARP_PIXEL_HSV_FLOAT);


            MAKE_2CASE(YARP_PIXEL_RGB_SIGNED,YARP_PIXEL_MONO_FLOAT);
            MAKE_2CASE(YARP_PIXEL_RGB_SIGNED,YARP_PIXEL_RGB_FLOAT);
            MAKE_2CASE(YARP_PIXEL_RGB_SIGNED,YARP_PIXEL_HSV_FLOAT);

            MAKE_2CASE(YARP_PIXEL_MONO_FLOAT,YARP_PIXEL_RGB_FLOAT);
            MAKE_2CASE(YARP_PIXEL_MONO_FLOAT,YARP_PIXEL_HSV_FLOAT);

            MAKE_2CASE(YARP_PIXEL_RGB_FLOAT,YARP_PIXEL_HSV_FLOAT);

            MAKE_2CASE(YARP_PIXEL_BGR,YARP_PIXEL_INT);

        default:
            ACE_OS::printf("*** Tried to copy type %d to %d\n", id1, id2);
            ACE_OS::exit(1);
            break;
        }
}

static int _GetPixelSize(int pixel_type)
{
    int result = 0;
    switch (pixel_type)
        {
        case YARP_PIXEL_MONO:
            result = sizeof(PixelMono);
            break;
        case YARP_PIXEL_RGB:
            result = sizeof(PixelRgb);
            break;
        case YARP_PIXEL_HSV:
            result = sizeof(PixelHsv);
            break;
        case YARP_PIXEL_BGR:
            result = sizeof(PixelBgr);
            break;
        case YARP_PIXEL_MONO_SIGNED:
            result = sizeof(PixelMonoSigned);
            break;
        case YARP_PIXEL_RGB_SIGNED:
            result = sizeof(PixelRgbSigned);
            break;
        case YARP_PIXEL_MONO_FLOAT:
            result = sizeof(PixelFloat);
            break;
        case YARP_PIXEL_RGB_FLOAT:
            result = sizeof(PixelRgbFloat);
            break;
        case YARP_PIXEL_HSV_FLOAT:
            result = sizeof(PixelHsvFloat);
            break;
        default:
            // only other acceptable possibility is that the size is being supplied
            // for an unknown type
            //ACE_ASSERT (pixel_type<0);
            result = -pixel_type;
            break;
        }
    //printf("Getting pixel size for %d (%d)\n", pixel_type, result);
    return result;
}
