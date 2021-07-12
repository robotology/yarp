/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/Vocab.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/impl/IplImage.h>

#include <cstring>
#include <cstdio>

using namespace yarp::sig;

#define DBG if(0)

// Default copy mechanism
template <class T1, class T2>
static inline void CopyPixel(const T1 *src, T2 *dest)
{
    *dest = *src;
}

/******************************************************************************/

static inline void CopyPixel(const PixelMono* src, PixelRgb* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
}

static inline void CopyPixel(const PixelMono* src, PixelRgba* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
    dest->a = 255;
}

static inline void CopyPixel(const PixelMono* src, PixelBgra* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
    dest->a = 255;
}

static inline void CopyPixel(const PixelMono* src, PixelRgbInt* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
}

static inline void CopyPixel(const PixelMono* src, PixelBgr* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
}

static inline void CopyPixel(const PixelMono* src, PixelHsv* dest)
{
    dest->v = *src;
    dest->h = 0;
    dest->s = 0;
}

static inline void CopyPixel(const PixelMono* src, PixelRgbSigned* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
}

static inline void CopyPixel(const PixelMono* src, PixelRgbFloat* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
}

static inline void CopyPixel(const PixelMono* src, PixelHsvFloat* dest)
{
    dest->v = *src;
    dest->h = 0;
    dest->s = 0;
}

static inline void CopyPixel(const PixelMono* src, PixelMonoSigned* dest)
{
    *dest = *src >> 1;
}

static inline void CopyPixel(const PixelMono* src, PixelInt* dest)
{
    *dest = *src;
}

static inline void CopyPixel(const PixelMono* src, PixelMono16* dest)
{
    *dest = *src;
}

static inline void CopyPixel(const PixelMono* src, PixelFloat* dest)
{
    *dest = *src;
}

/******************************************************************************/

static inline void CopyPixel(const PixelRgb* src, PixelMono* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgb* src, PixelMono16* dest)
{
    *dest = static_cast<yarp::sig::PixelMono16>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgb* src, PixelInt* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgb* src, PixelHsv* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelRgb* src, PixelMonoSigned* dest)
{
    *dest = static_cast<char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgb* src, PixelRgbSigned* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgb* src, PixelRgba* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
    dest->a = 255;
}

static inline void CopyPixel(const PixelRgb* src, PixelBgra* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
    dest->a = 255;
}

static inline void CopyPixel(const PixelRgb* src, PixelRgbInt* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgb* src, PixelFloat* dest)
{
    *dest = ((src->r + src->g + src->b)/3.0f);
}

static inline void CopyPixel(const PixelRgb* src, PixelRgbFloat* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgb* src, PixelBgr* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgb* src, PixelHsvFloat* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

/******************************************************************************/

static inline void CopyPixel(const PixelHsv* src, PixelMono* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsv* src, PixelMono16* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsv* src, PixelRgb* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsv* src, PixelRgba* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsv* src, PixelBgra* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsv* src, PixelRgbInt* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsv* src, PixelBgr* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsv* src, PixelMonoSigned* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsv* src, PixelRgbSigned* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsv* src, PixelFloat* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsv* src, PixelRgbFloat* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsv* src, PixelHsvFloat* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsv* src, PixelInt* dest)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    yAssert(false); // Not implemented yet
}

/******************************************************************************/

static inline void CopyPixel(const PixelBgr* src, PixelMono* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelBgr* src, PixelMono16* dest)
{
    *dest = static_cast<yarp::sig::PixelMono16>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelBgr* src, PixelInt* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelBgr* src, PixelHsv* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelBgr* src, PixelMonoSigned* dest)
{
    *dest = static_cast<char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelBgr* src, PixelRgbSigned* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelBgr* src, PixelFloat* dest)
{
    *dest = ((src->r + src->g + src->b)/3.0f);
}

static inline void CopyPixel(const PixelBgr* src, PixelRgbFloat* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelBgr* src, PixelRgb* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelBgr* src, PixelRgba* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
    dest->a = 255;
}

static inline void CopyPixel(const PixelBgr* src, PixelBgra* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
    dest->a = 255;
}

static inline void CopyPixel(const PixelBgr* src, PixelRgbInt* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelBgr* src, PixelHsvFloat* dest)
{
    yAssert(false); // Not implemented yet
}

/******************************************************************************/

static inline void CopyPixel(const PixelRgba* src, PixelMono* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgba* src, PixelMono16* dest)
{
    *dest = static_cast<yarp::sig::PixelMono16>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgba* src, PixelInt* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgba* src, PixelHsv* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelRgba* src, PixelMonoSigned* dest)
{
    *dest = static_cast<char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgba* src, PixelRgbSigned* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgba* src, PixelFloat* dest)
{
    *dest = ((src->r + src->g + src->b)/3.0f);
}

static inline void CopyPixel(const PixelRgba* src, PixelRgbFloat* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgba* src, PixelRgb* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgba* src, PixelBgra* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
    dest->a = src->a;
}

static inline void CopyPixel(const PixelRgba* src, PixelBgr* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgba* src, PixelRgbInt* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgba* src, PixelHsvFloat* dest)
{
    yAssert(false); // Not implemented yet
}

/******************************************************************************/

static inline void CopyPixel(const PixelBgra* src, PixelMono* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelBgra* src, PixelMono16* dest)
{
    *dest = static_cast<yarp::sig::PixelMono16>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelBgra* src, PixelInt* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelBgra* src, PixelHsv* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelBgra* src, PixelMonoSigned* dest)
{
    *dest = static_cast<char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelBgra* src, PixelRgbSigned* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelBgra* src, PixelFloat* dest)
{
    *dest = ((src->r + src->g + src->b)/3.0f);
}

static inline void CopyPixel(const PixelBgra* src, PixelRgbFloat* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelBgra* src, PixelRgb* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelBgra* src, PixelRgba* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
    dest->a = src->a;
}

static inline void CopyPixel(const PixelBgra* src, PixelBgr* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelBgra* src, PixelRgbInt* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelBgra* src, PixelHsvFloat* dest)
{
    yAssert(false); // Not implemented yet
}

/******************************************************************************/

static inline void CopyPixel(const PixelRgbInt* src, PixelMono* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbInt* src, PixelMono16* dest)
{
    *dest = static_cast<yarp::sig::PixelMono16>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbInt* src, PixelInt* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbInt* src, PixelHsv* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelRgbInt* src, PixelMonoSigned* dest)
{
    *dest = static_cast<char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbInt* src, PixelRgbSigned* dest)
{
    dest->r = static_cast<char>(src->r);
    dest->g = static_cast<char>(src->g);
    dest->b = static_cast<char>(src->b);
}

static inline void CopyPixel(const PixelRgbInt* src, PixelFloat* dest)
{
    *dest = ((src->r + src->g + src->b)/3.0f);
}

static inline void CopyPixel(const PixelRgbInt* src, PixelRgbFloat* dest)
{
    dest->r = static_cast<float>(static_cast<int>(src->r));
    dest->g = static_cast<float>(static_cast<int>(src->g));
    dest->b = static_cast<float>(static_cast<int>(src->b));
}

static inline void CopyPixel(const PixelRgbInt* src, PixelRgb* dest)
{
    dest->r = static_cast<unsigned char>(src->r);
    dest->g = static_cast<unsigned char>(src->g);
    dest->b = static_cast<unsigned char>(src->b);
}

static inline void CopyPixel(const PixelRgbInt* src, PixelBgr* dest)
{
    dest->r = static_cast<unsigned char>(src->r);
    dest->g = static_cast<unsigned char>(src->g);
    dest->b = static_cast<unsigned char>(src->b);
}

static inline void CopyPixel(const PixelRgbInt* src, PixelRgba* dest)
{
    dest->r = static_cast<unsigned char>(src->r);
    dest->g = static_cast<unsigned char>(src->g);
    dest->b = static_cast<unsigned char>(src->b);
    dest->a = 255;
}

static inline void CopyPixel(const PixelRgbInt* src, PixelBgra* dest)
{
    dest->r = static_cast<unsigned char>(src->r);
    dest->g = static_cast<unsigned char>(src->g);
    dest->b = static_cast<unsigned char>(src->b);
    dest->a = 255;
}

static inline void CopyPixel(const PixelRgbInt* src, PixelHsvFloat* dest)
{
    yAssert(false); // Not implemented yet
}

/******************************************************************************/

static inline void CopyPixel(const PixelMonoSigned* src, PixelRgb* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
}

static inline void CopyPixel(const PixelMonoSigned* src, PixelRgba* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
    dest->a = 255;
}

static inline void CopyPixel(const PixelMonoSigned* src, PixelBgra* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
    dest->a = 255;
}

static inline void CopyPixel(const PixelMonoSigned* src, PixelRgbInt* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
}

static inline void CopyPixel(const PixelMonoSigned* src, PixelBgr* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
}

static inline void CopyPixel(const PixelMonoSigned* src, PixelHsv* dest)
{
    dest->v = *src;
    dest->h = 0;
    dest->s = 0;
}

static inline void CopyPixel(const PixelMonoSigned* src, PixelRgbSigned* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
}

static inline void CopyPixel(const PixelMonoSigned* src, PixelRgbFloat* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
}

static inline void CopyPixel(const PixelMonoSigned* src, PixelHsvFloat* dest)
{
    dest->v = *src;
    dest->h = 0;
    dest->s = 0;
}

static inline void CopyPixel(const PixelMonoSigned* src, PixelMono* dest)
{
    *dest = *src + 128;
}

static inline void CopyPixel(const PixelMonoSigned* src, PixelInt* dest)
{
    *dest = *src;
}

static inline void CopyPixel(const PixelMonoSigned* src, PixelMono16* dest)
{
    *dest = static_cast<yarp::sig::PixelMono16>(*src);
}

/******************************************************************************/

static inline void CopyPixel(const PixelRgbSigned* src, PixelMono* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbSigned* src, PixelMono16* dest)
{
    *dest = static_cast<PixelMono16>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbSigned* src, PixelInt* dest)
{
    *dest = static_cast<int>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbSigned* src, PixelHsv* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelRgbSigned* src, PixelMonoSigned* dest)
{
    *dest = static_cast<char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbSigned* src, PixelRgb* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgbSigned* src, PixelRgba* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
    dest->a = 255;
}

static inline void CopyPixel(const PixelRgbSigned* src, PixelBgra* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
    dest->a = 255;
}

static inline void CopyPixel(const PixelRgbSigned* src, PixelRgbInt* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgbSigned* src, PixelBgr* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgbSigned* src, PixelFloat* dest)
{
    *dest = ((src->r + src->g + src->b)/3.0f);
}

static inline void CopyPixel(const PixelRgbSigned* src, PixelRgbFloat* dest)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

static inline void CopyPixel(const PixelRgbSigned* src, PixelHsvFloat* dest)
{
    yAssert(false); // Not implemented yet
}

/******************************************************************************/

static inline void CopyPixel(const PixelFloat* src, PixelMono* dest)
{
    *dest = static_cast<unsigned char>(*src);
}

static inline void CopyPixel(const PixelFloat* src, PixelMono16* dest)
{
    *dest = static_cast<yarp::sig::PixelMono16>(*src);
}

static inline void CopyPixel(const PixelFloat* src, PixelInt* dest)
{
    *dest = static_cast<unsigned char>(*src);
}

static inline void CopyPixel(const PixelFloat* src, PixelMonoSigned* dest)
{
    *dest = static_cast<char>(*src);
}

static inline void CopyPixel(const PixelFloat* src, PixelRgb* dest)
{
    dest->r = static_cast<unsigned char>(*src);
    dest->g = static_cast<unsigned char>(*src);
    dest->b = static_cast<unsigned char>(*src);
}

static inline void CopyPixel(const PixelFloat* src, PixelRgba* dest)
{
    dest->r = static_cast<unsigned char>(*src);
    dest->g = static_cast<unsigned char>(*src);
    dest->b = static_cast<unsigned char>(*src);
    dest->a = 255;
}

static inline void CopyPixel(const PixelFloat* src, PixelBgra* dest)
{
    dest->r = static_cast<unsigned char>(*src);
    dest->g = static_cast<unsigned char>(*src);
    dest->b = static_cast<unsigned char>(*src);
    dest->a = 255;
}

static inline void CopyPixel(const PixelFloat* src, PixelRgbInt* dest)
{
    dest->r = static_cast<int>(*src);
    dest->g = static_cast<int>(*src);
    dest->b = static_cast<int>(*src);
}

static inline void CopyPixel(const PixelFloat* src, PixelBgr* dest)
{
    dest->r = static_cast<unsigned char>(*src);
    dest->g = static_cast<unsigned char>(*src);
    dest->b = static_cast<unsigned char>(*src);
}

static inline void CopyPixel(const PixelFloat* src, PixelHsv* dest)
{
    dest->v = static_cast<unsigned char>(*src);
    dest->h = 0;
    dest->s = 0;
}

static inline void CopyPixel(const PixelFloat* src, PixelRgbSigned* dest)
{
    dest->r = static_cast<signed char>(*src);
    dest->g = static_cast<signed char>(*src);
    dest->b = static_cast<signed char>(*src);
}

static inline void CopyPixel(const PixelFloat* src, PixelRgbFloat* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
}

static inline void CopyPixel(const PixelFloat* src, PixelHsvFloat* dest)
{
    dest->v = *src;
    dest->h = 0;
    dest->s = 0;
}

/******************************************************************************/

static inline void CopyPixel(const PixelRgbFloat* src, PixelMono* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbFloat* src, PixelInt* dest)
{
    *dest = static_cast<unsigned char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbFloat* src, PixelMono16* dest)
{
    *dest = static_cast<yarp::sig::PixelMono16>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbFloat* src, PixelHsv* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelRgbFloat* src, PixelMonoSigned* dest)
{
    *dest = static_cast<char>((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbFloat* src, PixelRgb* dest)
{
    dest->r = static_cast<unsigned char>(src->r);
    dest->g = static_cast<unsigned char>(src->g);
    dest->b = static_cast<unsigned char>(src->b);
}

static inline void CopyPixel(const PixelRgbFloat* src, PixelRgba* dest)
{
    dest->r = static_cast<unsigned char>(src->r);
    dest->g = static_cast<unsigned char>(src->g);
    dest->b = static_cast<unsigned char>(src->b);
    dest->a = 255;
}

static inline void CopyPixel(const PixelRgbFloat* src, PixelBgra* dest)
{
    dest->r = static_cast<unsigned char>(src->r);
    dest->g = static_cast<unsigned char>(src->g);
    dest->b = static_cast<unsigned char>(src->b);
    dest->a = 255;
}

static inline void CopyPixel(const PixelRgbFloat* src, PixelRgbInt* dest)
{
    dest->r = static_cast<int>(src->r);
    dest->g = static_cast<int>(src->g);
    dest->b = static_cast<int>(src->b);
}

static inline void CopyPixel(const PixelRgbFloat* src, PixelBgr* dest)
{
    dest->r = static_cast<unsigned char>(src->r);
    dest->g = static_cast<unsigned char>(src->g);
    dest->b = static_cast<unsigned char>(src->b);
}

static inline void CopyPixel(const PixelRgbFloat* src, PixelFloat* dest)
{
    *dest = ((src->r + src->g + src->b)/3);
}

static inline void CopyPixel(const PixelRgbFloat* src, PixelRgbSigned* dest)
{
    dest->r = static_cast<signed char>(src->r);
    dest->g = static_cast<signed char>(src->g);
    dest->b = static_cast<signed char>(src->b);
}

static inline void CopyPixel(const PixelRgbFloat* src, PixelHsvFloat* dest)
{
    yAssert(false); // Not implemented yet
}

/******************************************************************************/

static inline void CopyPixel(const PixelHsvFloat* src, PixelMono* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsvFloat* src, PixelMono16* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsvFloat* src, PixelRgb* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsvFloat* src, PixelBgr* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsvFloat* src, PixelRgba* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsvFloat* src, PixelBgra* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsvFloat* src, PixelRgbInt* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsvFloat* src, PixelMonoSigned* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsvFloat* src, PixelRgbSigned* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsvFloat* src, PixelFloat* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsvFloat* src, PixelRgbFloat* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsvFloat* src, PixelHsv* dest)
{
    yAssert(false); // Not implemented yet
}

static inline void CopyPixel(const PixelHsvFloat* src, PixelInt* dest)
{
    yAssert(false); // Not implemented yet
}

/******************************************************************************/

static inline void CopyPixel(const PixelInt* src, PixelRgb* dest)
{
    dest->r = static_cast<char>(*src);
    dest->g = static_cast<char>(*src);
    dest->b = static_cast<char>(*src);
}

static inline void CopyPixel(const PixelInt* src, PixelRgba* dest)
{
    dest->r = static_cast<char>(*src);
    dest->g = static_cast<char>(*src);
    dest->b = static_cast<char>(*src);
    dest->a = 255;
}

static inline void CopyPixel(const PixelInt* src, PixelBgra* dest)
{
    dest->r = static_cast<char>(*src);
    dest->g = static_cast<char>(*src);
    dest->b = static_cast<char>(*src);
    dest->a = 255;
}

static inline void CopyPixel(const PixelInt* src, PixelRgbInt* dest)
{
    dest->r = *src;
    dest->g = *src;
    dest->b = *src;
}

static inline void CopyPixel(const PixelInt* src, PixelBgr* dest)
{
    dest->r = static_cast<char>(*src);
    dest->g = static_cast<char>(*src);
    dest->b = static_cast<char>(*src);
}

static inline void CopyPixel(const PixelInt* src, PixelHsv* dest)
{
    dest->v = static_cast<yarp::sig::PixelMono>(*src);
    dest->h = dest->s = 0;
}

static inline void CopyPixel(const PixelInt* src, PixelRgbSigned* dest)
{
    dest->r = static_cast<yarp::sig::PixelMono>(*src);
    dest->g = static_cast<yarp::sig::PixelMono>(*src);
    dest->b = static_cast<yarp::sig::PixelMono>(*src);
}

static inline void CopyPixel(const PixelInt* src, PixelFloat* dest)
{
    *dest = static_cast<float>(*src);
}

static inline void CopyPixel(const PixelInt* src, PixelRgbFloat* dest)
{
    dest->r = static_cast<float>(*src);
    dest->g = static_cast<float>(*src);
    dest->b = static_cast<float>(*src);
}

static inline void CopyPixel(const PixelInt* src, PixelHsvFloat* dest)
{
    dest->v = float(*src);
    dest->h = 0;
    dest->s = 0;
}

static inline void CopyPixel(const PixelInt* src, PixelMonoSigned* dest)
{
    *dest = static_cast<char>(*src >> 1);
}

static inline void CopyPixel(const PixelInt* src, PixelMono* dest)
{
    *dest = static_cast<yarp::sig::PixelMono>(*src);
}

static inline void CopyPixel(const PixelInt* src, PixelMono16* dest)
{
    *dest = static_cast<yarp::sig::PixelMono16>(*src);
}

/******************************************************************************/

static inline void CopyPixel(const PixelMono16* src, PixelRgb* dest)
{
    dest->r = static_cast<char>(*src);
    dest->g = static_cast<char>(*src);
    dest->b = static_cast<char>(*src);
}

static inline void CopyPixel(const PixelMono16* src, PixelRgba* dest)
{
    dest->r = static_cast<char>(*src);
    dest->g = static_cast<char>(*src);
    dest->b = static_cast<char>(*src);
    dest->a = 255;
}

static inline void CopyPixel(const PixelMono16* src, PixelBgra* dest)
{
    dest->r = static_cast<char>(*src);
    dest->g = static_cast<char>(*src);
    dest->b = static_cast<char>(*src);
    dest->a = 255;
}

static inline void CopyPixel(const PixelMono16* src, PixelRgbInt* dest)
{
    dest->r = static_cast<int>(static_cast<unsigned>(*src));
    dest->g = static_cast<int>(static_cast<unsigned>(*src));
    dest->b = static_cast<int>(static_cast<unsigned>(*src));
}

static inline void CopyPixel(const PixelMono16* src, PixelInt* dest)
{
  *dest = static_cast<int>(static_cast<unsigned>(*src));
}

static inline void CopyPixel(const PixelMono16* src, PixelBgr* dest)
{
    dest->r = static_cast<char>(*src);
    dest->g = static_cast<char>(*src);
    dest->b = static_cast<char>(*src);
}

static inline void CopyPixel(const PixelMono16* src, PixelHsv* dest)
{
    dest->v = static_cast<yarp::sig::PixelMono>(*src);
    dest->h = 0;
    dest->s = 0;
}

static inline void CopyPixel(const PixelMono16* src, PixelRgbSigned* dest)
{
    dest->r = static_cast<yarp::sig::PixelMono>(*src);
    dest->g = static_cast<yarp::sig::PixelMono>(*src);
    dest->b = static_cast<yarp::sig::PixelMono>(*src);
}

static inline void CopyPixel(const PixelMono16* src, PixelFloat* dest)
{
    *dest = static_cast<float>(*src);
}

static inline void CopyPixel(const PixelMono16* src, PixelRgbFloat* dest)
{
    dest->r = static_cast<float>(*src);
    dest->g = static_cast<float>(*src);
    dest->b = static_cast<float>(*src);
}

static inline void CopyPixel(const PixelMono16* src, PixelHsvFloat* dest)
{
    dest->v = static_cast<float>(*src);
    dest->h = 0;
    dest->s = 0;
}

static inline void CopyPixel(const PixelMono16* src, PixelMonoSigned* dest)
{
    *dest = static_cast<char>(*src >> 1);
}

static inline void CopyPixel(const PixelMono16* src, PixelMono* dest)
{
    *dest = static_cast<yarp::sig::PixelMono>(*src);
}

static inline void CopyPixel(const PixelInt* src, PixelInt* dest)
{
    *dest = *src;
}

/******************************************************************************/


//static inline int PAD_BYTES (int len, int pad)
//{
//    const int rem = len % pad;
//    return (rem != 0) ? (pad - rem) : rem;
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
        odest = reinterpret_cast<T2*>(((char *)odest) + step2*(h-1));
        dest = odest;
    }

    for (int i=0; i<h; i++) {
        DBG printf("x,y = %d,%d\n", 0,i);
        for (int j = 0; j < w; j++) {
            CopyPixel(src,dest);
            src++;
            dest++;
        }

        src = reinterpret_cast<const T1*>(((char *)src) + p1);
        odest = reinterpret_cast<T2*>(((char *)odest) + step2*(flip?-1:1));
        dest = odest;
    }
}


using Def_VOCAB_PIXEL_MONO = PixelMono;
using Def_VOCAB_PIXEL_MONO16 = PixelMono16;
using Def_VOCAB_PIXEL_RGB = PixelRgb;
using Def_VOCAB_PIXEL_RGBA = PixelRgba;
using Def_VOCAB_PIXEL_BGRA = PixelBgra;
using Def_VOCAB_PIXEL_HSV = PixelHsv;
using Def_VOCAB_PIXEL_BGR = PixelBgr;
using Def_VOCAB_PIXEL_MONO_SIGNED = PixelMonoSigned;
using Def_VOCAB_PIXEL_RGB_SIGNED = PixelRgbSigned;
using Def_VOCAB_PIXEL_MONO_FLOAT = PixelFloat;
using Def_VOCAB_PIXEL_RGB_FLOAT = PixelRgbFloat;
using Def_VOCAB_PIXEL_HSV_FLOAT = PixelHsvFloat;
using Def_VOCAB_PIXEL_INT = PixelInt;
using Def_VOCAB_PIXEL_RGB_INT = PixelRgbInt;

#define HASH(id1, id2) ((int)(((int)(id1%65537))*11 + ((long int)(id2))))
#define HANDLE_CASE(len, x1, T1, q1, o1, x2, T2, q2, o2) CopyPixels(reinterpret_cast<const T1*>(x1), q1, reinterpret_cast<T2*>(x2), q2, w, h, o1!=o2);
#define MAKE_CASE(id1, id2) case HASH(id1, id2): HANDLE_CASE(len, src, Def_##id1, quantum1, topIsLow1, dest, Def_##id2, quantum2, topIsLow2); break;

// More elegant ways to do this, but needs to be efficient at pixel level
void Image::copyPixels(const unsigned char *src, size_t id1,
                       char unsigned *dest, size_t id2, size_t w, size_t h,
                       size_t imageSize, size_t quantum1, size_t quantum2,
                       bool topIsLow1, bool topIsLow2)
{
    DBG printf("copyPixels...\n");

    if (id1==id2&&quantum1==quantum2&&topIsLow1==topIsLow2) {
        memcpy(dest,src,imageSize);
        return;
    }


    switch(HASH(id1,id2)) {
        // Macros rely on len, x1, x2 variable names

        // Each MAKE_CASE line here expands to something like this:
        //
        // case HASH(VOCAB_PIXEL_MONO, VOCAB_PIXEL_RGB):
        //     CopyPixels(reinterpret_cast<const PixelMono*>(src), quantum1,
        //                reinterpret_cast<PixelRgb*>(dest), quantum2,
        //                w, h, topIsLow1!=topIsLow2);
        //     break;

        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_MONO, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_RGB, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_HSV, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_BGR, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_MONO_SIGNED, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_RGBA, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_BGRA, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_RGB_SIGNED, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_MONO_FLOAT, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_RGB_FLOAT, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_HSV_FLOAT, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_INT, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_RGB_INT, VOCAB_PIXEL_MONO16)

        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_MONO)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_RGB)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_HSV)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_BGR)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_MONO_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_RGBA)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_BGRA)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_RGB_SIGNED)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_MONO_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_RGB_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_HSV_FLOAT)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_INT)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_RGB_INT)
        MAKE_CASE(VOCAB_PIXEL_MONO16, VOCAB_PIXEL_MONO16)

        default:
            printf("*** Tried to copy type %s to %s\n",
                   yarp::os::Vocab32::decode(id1).c_str(),
                   yarp::os::Vocab32::decode(id2).c_str());
            std::exit(1);
            break;
    }

    DBG printf("... done copyPixels\n");
}
