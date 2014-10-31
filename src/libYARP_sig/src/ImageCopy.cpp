// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <string.h>
#include <yarp/os/Log.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/IplImage.h>

using namespace yarp::sig;

#define DBG if(0)

// Default copy mechanism
template <class T1, class T2>
static inline void CopyPixel(const T1 *src, T2 *dest)
{
	*dest = *src;
}

typedef PixelMono Def_VOCAB_PIXEL_MONO;
typedef PixelMono16 Def_VOCAB_PIXEL_MONO16;
typedef PixelRgb Def_VOCAB_PIXEL_RGB;
typedef PixelRgba Def_VOCAB_PIXEL_RGBA;
typedef PixelBgra Def_VOCAB_PIXEL_BGRA;
typedef PixelHsv Def_VOCAB_PIXEL_HSV;
typedef PixelBgr Def_VOCAB_PIXEL_BGR;
typedef PixelMonoSigned Def_VOCAB_PIXEL_MONO_SIGNED;
typedef PixelRgbSigned Def_VOCAB_PIXEL_RGB_SIGNED;
typedef PixelFloat Def_VOCAB_PIXEL_MONO_FLOAT;
typedef PixelRgbFloat Def_VOCAB_PIXEL_RGB_FLOAT;
typedef PixelHsvFloat Def_VOCAB_PIXEL_HSV_FLOAT;
typedef PixelInt Def_VOCAB_PIXEL_INT;
typedef PixelRgbInt Def_VOCAB_PIXEL_RGB_INT;

#define VALID_PIXEL(x) ((x>255)?255:((x<0)?0:x))
#define SPECIAL_COPY_BEGIN static void YARPDummyCopyPixel() {
#define SPECIAL_COPY(id1,id2) } static inline void CopyPixel(const Def_##id1 *src, Def_##id2 *dest) {
#define SPECIAL_COPY_END }

static int implemented_yet = 1;

SPECIAL_COPY_BEGIN

SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_RGB)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_RGBA)
    dest->r = dest->g = dest->b = *src; 
    dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_BGRA)
    dest->r = dest->g = dest->b = *src; 
    dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_RGB_INT)
    dest->r = dest->g = dest->b = *src; 
SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_BGR)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_HSV)
    dest->v = *src;
dest->h = dest->s = 0;
SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_RGB_SIGNED)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_RGB_FLOAT)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_HSV_FLOAT)
    dest->v = *src;
dest->h = dest->s = 0;
SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_MONO_SIGNED)
    *dest = *src >> 1;
SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_INT)
    *dest = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_MONO16)
    *dest = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO,VOCAB_PIXEL_MONO_FLOAT)
    *dest = *src;

SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_MONO)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_MONO16)
    *dest = (yarp::sig::PixelMono16)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_INT)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_HSV)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_MONO_SIGNED)
    *dest = (char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_RGB_SIGNED)
    dest->r = src->r; dest->g = src->g; dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_RGBA)
    dest->r = src->r; dest->g = src->g; dest->b = src->b;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_BGRA)
    dest->r = src->r; dest->g = src->g; dest->b = src->b;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_RGB_INT)
    dest->r = src->r; dest->g = src->g; dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_MONO_FLOAT)
    *dest = ((src->r + src->g + src->b)/3.0f);
SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_RGB_FLOAT)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_BGR)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB,VOCAB_PIXEL_HSV_FLOAT)
    yAssert(implemented_yet == 0);


SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_MONO)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_MONO16)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_RGB)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_RGBA)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_BGRA)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_RGB_INT)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_BGR)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_MONO_SIGNED)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_RGB_SIGNED)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_MONO_FLOAT)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_RGB_FLOAT)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_HSV_FLOAT)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV,VOCAB_PIXEL_INT)
    yAssert(implemented_yet == 0);


SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_MONO)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_MONO16)
    *dest = (yarp::sig::PixelMono16)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_INT)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_HSV)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_MONO_SIGNED)
    *dest = (char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_RGB_SIGNED)
    dest->r = src->r; dest->g = src->g; dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_MONO_FLOAT)
    *dest = ((src->r + src->g + src->b)/3.0f);
SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_RGB_FLOAT)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_RGB)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_RGBA)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_BGRA)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_RGB_INT)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_BGR,VOCAB_PIXEL_HSV_FLOAT)
    yAssert(implemented_yet == 0);



SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_MONO)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_MONO16)
    *dest = (yarp::sig::PixelMono16)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_INT)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_HSV)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_MONO_SIGNED)
    *dest = (char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_RGB_SIGNED)
    dest->r = src->r; dest->g = src->g; dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_MONO_FLOAT)
    *dest = ((src->r + src->g + src->b)/3.0f);
SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_RGB_FLOAT)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_RGB)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_BGRA)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
dest->a = src->a;
SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_BGR)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_RGB_INT)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_HSV_FLOAT)
    yAssert(implemented_yet == 0);


SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_MONO)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_MONO16)
    *dest = (yarp::sig::PixelMono16)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_INT)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_HSV)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_MONO_SIGNED)
    *dest = (char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_RGB_SIGNED)
    dest->r = src->r; dest->g = src->g; dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_MONO_FLOAT)
    *dest = ((src->r + src->g + src->b)/3.0f);
SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_RGB_FLOAT)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_RGB)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_RGBA)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
dest->a = src->a;
SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_BGR)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_RGB_INT)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_HSV_FLOAT)
    yAssert(implemented_yet == 0);



SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_MONO)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_MONO16)
    *dest = (yarp::sig::PixelMono16)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_INT)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_HSV)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_MONO_SIGNED)
    *dest = (char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_RGB_SIGNED)
    dest->r = (char)src->r; dest->g = (char)src->g; dest->b = (char)src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_MONO_FLOAT)
    *dest = ((src->r + src->g + src->b)/3.0f);
SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_RGB_FLOAT)
    dest->r = (float)((int) src->r);
dest->g = (float)((int) src->g);
dest->b = (float)((int) src->b);
SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_RGB)
    dest->r = (unsigned char)src->r;
dest->g = (unsigned char)src->g;
dest->b = (unsigned char)src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_BGR)
    dest->r = (unsigned char)src->r;
dest->g = (unsigned char)src->g;
dest->b = (unsigned char)src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_RGBA)
    dest->r = (unsigned char)src->r;
dest->g = (unsigned char)src->g;
dest->b = (unsigned char)src->b;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_BGRA)
    dest->r = (unsigned char)src->r;
dest->g = (unsigned char)src->g;
dest->b = (unsigned char)src->b;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_HSV_FLOAT)
    yAssert(implemented_yet == 0);



SPECIAL_COPY(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_RGB)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_RGBA)
    dest->r = dest->g = dest->b = *src; dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_BGRA)
    dest->r = dest->g = dest->b = *src; dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_RGB_INT)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_BGR)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_HSV)
    dest->v = *src;
dest->h = dest->s = 0;
SPECIAL_COPY(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_RGB_SIGNED)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_RGB_FLOAT)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_HSV_FLOAT)
    dest->v = *src;
dest->h = dest->s = 0;
SPECIAL_COPY(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_MONO)
    *dest = *src + 128;
SPECIAL_COPY(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_INT)
    *dest = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_MONO16)
    *dest = (yarp::sig::PixelMono16)(*src);

SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_MONO)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_MONO16)
    *dest = (PixelMono16)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_INT)
    *dest = (int)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_HSV)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_MONO_SIGNED)
    *dest = (char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_RGB)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_RGBA)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_BGRA)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_RGB_INT)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_BGR)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_MONO_FLOAT)
    *dest = ((src->r + src->g + src->b)/3.0f);
SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_RGB_FLOAT)
    dest->r = src->r;
dest->g = src->g;
dest->b = src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_HSV_FLOAT)
    yAssert(implemented_yet == 0);

SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_MONO)
    *dest = (unsigned char)*src;
SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_MONO16)
    *dest = (yarp::sig::PixelMono16)*src;
SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_INT)
    *dest = (unsigned char)*src;
SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_MONO_SIGNED)
    *dest = (char)*src;
SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_RGB)
    dest->r = dest->g = dest->b = (unsigned char)(*src);
SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_RGBA)
    dest->r = dest->g = dest->b = (unsigned char)(*src);
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_BGRA)
    dest->r = dest->g = dest->b = (unsigned char)(*src);
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_RGB_INT)
    dest->r = dest->g = dest->b = (int)(*src);
SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_BGR)
    dest->r = dest->g = dest->b = (unsigned char)(*src);
SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_HSV)
    dest->v = (unsigned char)*src;
dest->h = dest->s = 0;
SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_RGB_SIGNED)
    dest->r = dest->g = dest->b = (signed char) *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_RGB_FLOAT)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_HSV_FLOAT)
    dest->v = *src;
dest->h = dest->s = 0;

SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_MONO)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_INT)
    *dest = (unsigned char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_MONO16)
    *dest = (yarp::sig::PixelMono16)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_HSV)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_MONO_SIGNED)
    *dest = (char)((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_RGB)
    dest->r = (unsigned char) src->r;
dest->g = (unsigned char) src->g;
dest->b = (unsigned char) src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_RGBA)
    dest->r = (unsigned char) src->r;
dest->g = (unsigned char) src->g;
dest->b = (unsigned char) src->b;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_BGRA)
    dest->r = (unsigned char) src->r;
dest->g = (unsigned char) src->g;
dest->b = (unsigned char) src->b;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_RGB_INT)
    dest->r = (int) src->r;
dest->g = (int) src->g;
dest->b = (int) src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_BGR)
    dest->r = (unsigned char) src->r;
dest->g = (unsigned char) src->g;
dest->b = (unsigned char) src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_MONO_FLOAT)
    *dest = ((src->r + src->g + src->b)/3);
SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_RGB_SIGNED)
    dest->r = (signed char) src->r;
dest->g = (signed char) src->g;
dest->b = (signed char) src->b;
SPECIAL_COPY(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_HSV_FLOAT)
    yAssert(implemented_yet == 0);

SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_MONO)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_MONO16)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_RGB)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_BGR)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_RGBA)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_BGRA)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_RGB_INT)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_MONO_SIGNED)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_RGB_SIGNED)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_MONO_FLOAT)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_RGB_FLOAT)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_HSV)
    yAssert(implemented_yet == 0);
SPECIAL_COPY(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_INT)
    yAssert(implemented_yet == 0);

SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_RGB)
    dest->r = dest->g = dest->b = (char)*src;
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_RGBA)
    dest->r = dest->g = dest->b = (char)*src;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_BGRA)
    dest->r = dest->g = dest->b = (char)*src;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_RGB_INT)
    dest->r = dest->g = dest->b = *src;
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_BGR)
    dest->r = dest->g = dest->b = (char)*src;
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_HSV)
    dest->v = (yarp::sig::PixelMono)(*src);
dest->h = dest->s = 0;
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_RGB_SIGNED)
    dest->r = dest->g = dest->b = (yarp::sig::PixelMono)(*src);
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_MONO_FLOAT)
    *dest = (float)(*src);
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_RGB_FLOAT)
    dest->r = dest->g = dest->b = (float)(*src);
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_HSV_FLOAT)
    dest->v = float(*src);
dest->h = dest->s = 0;
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_MONO_SIGNED)
    *dest = (char)(*src >> 1);
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_MONO)
    *dest = (yarp::sig::PixelMono)(*src);
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_MONO16)
    *dest = (yarp::sig::PixelMono16)(*src);

SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_RGB)
    dest->r = dest->g = dest->b = (char)*src;
SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_RGBA)
    dest->r = dest->g = dest->b = (char)*src;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_BGRA)
    dest->r = dest->g = dest->b = (char)*src;
dest->a = 255;
SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_RGB_INT)
    dest->r = dest->g = dest->b = (int)((unsigned) *src);
SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_INT)
  *dest = (int)((unsigned) *src);
SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_BGR)
    dest->r = dest->g = dest->b = (char)*src;
SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_HSV)
    dest->v = (yarp::sig::PixelMono)(*src);
dest->h = dest->s = 0;
SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_RGB_SIGNED)
    dest->r = dest->g = dest->b = (yarp::sig::PixelMono)(*src);
SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_MONO_FLOAT)
    *dest = (float)(*src);
SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_RGB_FLOAT)
    dest->r = dest->g = dest->b = (float)(*src);
SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_HSV_FLOAT)
    dest->v = float(*src);
dest->h = dest->s = 0;
SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_MONO_SIGNED)
    *dest = (char)(*src >> 1);
SPECIAL_COPY(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_MONO)
    *dest = (yarp::sig::PixelMono)(*src);
SPECIAL_COPY(VOCAB_PIXEL_INT,VOCAB_PIXEL_INT)
    *dest = *src;

SPECIAL_COPY_END

//static inline int PAD_BYTES (int len, int pad)
//{
//	const int rem = len % pad;
//	return (rem != 0) ? (pad - rem) : rem;
//}

///
///
template <class T1, class T2>
static void CopyPixels(const T1 *osrc, int q1, T2 *odest, int q2, 
                       int w, int h,
                       bool flip)
{
    const T1 *src = osrc;
    T2 *dest = odest;
	const int p1 = PAD_BYTES (w * sizeof(T1), q1);
	const int p2 = PAD_BYTES (w * sizeof(T2), q2);
    //const int step1 = w*sizeof(T1) + p1;
    const int step2 = w*sizeof(T2) + p2;
    DBG printf("q1 %d q2 %d (%dx%d) inc %d %d\n", q1, q2, w, h, p1, p2);

    if (flip) {
        odest = (T2*)(((char *)odest) + step2*(h-1));
        dest = odest;
    }

    YARPDummyCopyPixel();
	for (int i=0; i<h; i++)
        {
            DBG printf("x,y = %d,%d\n", 0,i);
            for (int j = 0; j < w; j++)
                {
                    CopyPixel(src,dest);
                    src++;
                    dest++;
                }

            src = (const T1*)(((char *)src) + p1);
            odest = (T2*)(((char *)odest) + step2*(flip?-1:1));
            dest = odest;
        }
}


#define HASH(id1,id2) ((int)(((int)(id1%65537))*11+((long int)(id2))))
#define HANDLE_CASE(len,x1,T1,q1,o1,x2,T2,q2,o2) CopyPixels((T1*)x1,q1,(T2*)x2,q2,w,h,o1!=o2);
#define MAKE_CASE(id1,id2) case HASH(id1,id2): HANDLE_CASE(len,src,Def_##id1,quantum1,topIsLow1,dest,Def_##id2,quantum2,topIsLow2); break;
#define MAKE_2CASE(id1,id2) MAKE_CASE(id1,id2); MAKE_CASE(id2,id1);

// More elegant ways to do this, but needs to be efficient at pixel level
void Image::copyPixels(const unsigned char *src, int id1, 
                       char unsigned *dest, int id2, int w, int h,
                       int imageSize, int quantum1, int quantum2,
                       bool topIsLow1, bool topIsLow2)
{
    DBG printf("copyPixels...\n");

    if (id1==id2&&quantum1==quantum2&&topIsLow1==topIsLow2) {
        memcpy(dest,src,imageSize);
        return;
    }


    switch(HASH(id1,id2))
        {
            // Macros rely on len, x1, x2 variable names

            MAKE_CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_MONO);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_RGB);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_RGBA);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_HSV);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_BGR);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_MONO_SIGNED);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_RGB_SIGNED);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_MONO_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_RGB_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_HSV_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_INT);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_RGB_INT);
            MAKE_2CASE(VOCAB_PIXEL_MONO,VOCAB_PIXEL_MONO16);

            MAKE_CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_RGB);
            MAKE_2CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_HSV);
            MAKE_2CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_BGR);
            MAKE_2CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_RGBA);
            MAKE_2CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_MONO_SIGNED);
            MAKE_2CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_RGB_SIGNED);
            MAKE_2CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_MONO_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_RGB_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_HSV_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_INT);
            MAKE_2CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_RGB_INT);
            MAKE_2CASE(VOCAB_PIXEL_RGB,VOCAB_PIXEL_MONO16);

            MAKE_CASE(VOCAB_PIXEL_HSV,VOCAB_PIXEL_HSV);
            MAKE_2CASE(VOCAB_PIXEL_HSV,VOCAB_PIXEL_BGR);
            MAKE_2CASE(VOCAB_PIXEL_HSV,VOCAB_PIXEL_RGBA);
            MAKE_2CASE(VOCAB_PIXEL_HSV,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_HSV,VOCAB_PIXEL_MONO_SIGNED);
            MAKE_2CASE(VOCAB_PIXEL_HSV,VOCAB_PIXEL_RGB_SIGNED);
            MAKE_2CASE(VOCAB_PIXEL_HSV,VOCAB_PIXEL_MONO_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_HSV,VOCAB_PIXEL_RGB_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_HSV,VOCAB_PIXEL_HSV_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_HSV,VOCAB_PIXEL_INT);
            MAKE_2CASE(VOCAB_PIXEL_HSV,VOCAB_PIXEL_RGB_INT);
            MAKE_2CASE(VOCAB_PIXEL_HSV,VOCAB_PIXEL_MONO16);

            MAKE_CASE(VOCAB_PIXEL_BGR,VOCAB_PIXEL_BGR);
            MAKE_2CASE(VOCAB_PIXEL_BGR,VOCAB_PIXEL_MONO_SIGNED);
            MAKE_2CASE(VOCAB_PIXEL_BGR,VOCAB_PIXEL_RGBA);
            MAKE_2CASE(VOCAB_PIXEL_BGR,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_BGR,VOCAB_PIXEL_RGB_SIGNED);
            MAKE_2CASE(VOCAB_PIXEL_BGR,VOCAB_PIXEL_MONO_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_BGR,VOCAB_PIXEL_RGB_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_BGR,VOCAB_PIXEL_HSV_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_BGR,VOCAB_PIXEL_INT);
            MAKE_2CASE(VOCAB_PIXEL_BGR,VOCAB_PIXEL_RGB_INT);
            MAKE_2CASE(VOCAB_PIXEL_BGR,VOCAB_PIXEL_MONO16);

            MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_MONO_SIGNED);
            MAKE_2CASE(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_RGBA);
            MAKE_2CASE(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_RGB_SIGNED);
            MAKE_2CASE(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_MONO_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_RGB_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_HSV_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_INT);
            MAKE_2CASE(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_RGB_INT);
            MAKE_2CASE(VOCAB_PIXEL_MONO_SIGNED,VOCAB_PIXEL_MONO16);


            MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_RGB_SIGNED);
            MAKE_2CASE(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_RGBA);
            MAKE_2CASE(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_MONO_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_RGB_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_HSV_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_INT);
            MAKE_2CASE(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_RGB_INT);
            MAKE_2CASE(VOCAB_PIXEL_RGB_SIGNED,VOCAB_PIXEL_MONO16);

            MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_MONO_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_RGBA);
            MAKE_2CASE(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_RGB_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_HSV_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_INT);
            MAKE_2CASE(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_RGB_INT);
            MAKE_2CASE(VOCAB_PIXEL_MONO_FLOAT,VOCAB_PIXEL_MONO16);

            MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_RGB_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_RGBA);
            MAKE_2CASE(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_HSV_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_INT);
            MAKE_2CASE(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_RGB_INT);
            MAKE_2CASE(VOCAB_PIXEL_RGB_FLOAT,VOCAB_PIXEL_MONO16);

            MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_HSV_FLOAT);
            MAKE_2CASE(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_RGBA);
            MAKE_2CASE(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_INT);
            MAKE_2CASE(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_RGB_INT);
            MAKE_2CASE(VOCAB_PIXEL_HSV_FLOAT,VOCAB_PIXEL_MONO16);

            MAKE_CASE(VOCAB_PIXEL_INT,VOCAB_PIXEL_INT);
            MAKE_2CASE(VOCAB_PIXEL_INT,VOCAB_PIXEL_RGBA);
            MAKE_2CASE(VOCAB_PIXEL_INT,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_INT,VOCAB_PIXEL_RGB_INT);
            MAKE_2CASE(VOCAB_PIXEL_INT,VOCAB_PIXEL_MONO16);

            MAKE_CASE(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_RGBA);
            MAKE_2CASE(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_RGB_INT);
            MAKE_2CASE(VOCAB_PIXEL_RGBA,VOCAB_PIXEL_MONO16);

            MAKE_CASE(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_RGB_INT);
            MAKE_2CASE(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_RGB_INT,VOCAB_PIXEL_MONO16);

            MAKE_CASE(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_BGRA);
            MAKE_2CASE(VOCAB_PIXEL_BGRA,VOCAB_PIXEL_MONO16);

            MAKE_CASE(VOCAB_PIXEL_MONO16,VOCAB_PIXEL_MONO16);

        default:
            printf("*** Tried to copy type %d to %d\n", id1, id2);
            exit(1);
            break;
        }

    DBG printf("... done copyPixels\n");
}

