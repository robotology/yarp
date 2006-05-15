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
/// $Id: YARPImageDraw.h,v 1.2 2006-05-15 15:57:59 eshuy Exp $
///
///

#ifndef YARPImageDraw_INC
#define YARPImageDraw_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <math.h>

#include <yarp/YARPImage.h>

#ifndef M_PI
#define M_PI 3.1415926535897932
#endif

extern const YarpPixelRGB RGB_WHITE;
extern const YarpPixelRGB RGB_BLACK;
extern const YarpPixelRGB RGB_RED;
extern const YarpPixelRGB RGB_GREEN;
extern const YarpPixelRGB RGB_BLUE;
extern const YarpPixelRGB RGB_YELLOW;

template <class T>
void AddSegment(YARPImageOf<T>& dest, const T& pix, int x, int y, int dx, int dy)
{
	const double vx = double(dx - x);
	const double vy = double(dy - y);

	const int steps = (fabs(vx) > fabs(vy)) ? 2*fabs(vx) : 2*fabs(vy);
	const double r = 1.0 / steps;

	for (int i = 0; i <= steps; i++)
        {
            dest.SafePixel(int(x+vx*i*r),int(y+vy*i*r)) = pix;
        }
}

template <class T>
void AddCircle(YARPImageOf<T>& dest, const T& pix, int i, int j, int r)
{
	float d, r2 = r*r;
	for (int ii=i-r; ii<=i+r; ii++)
        {
            for (int jj=j-r; jj<=j+r; jj++)
                {
                    d = (ii-i)*(ii-i)+(jj-j)*(jj-j);
                    if (d<=r2)
                        {
                            dest.SafePixel(ii,jj) = pix;
                        }
                }
        }
}

template <class T>
void AddCrossHair(YARPImageOf<T>& dest, const T& pix, int i, int j, int r)
{
	for (int ii=i-r; ii<=i+r; ii++)
        {
            for (int jj=j-r; jj<=j+r; jj++)
                {
                    if (ii==i||jj==j)
                        {
                            dest.SafePixel(ii,jj) = pix;
                        }
                }
        }
}

template <class T>
void AddCircleOutline(YARPImageOf<T>& dest, const T& pix, int i, int j, int r)
{
	float d, r2 = r*r, r2l = (r-1.1)*(r-1.1);
	for (int ii=i-r; ii<=i+r; ii++)
        {
            for (int jj=j-r; jj<=j+r; jj++)
                {
                    d = (ii-i)*(ii-i)+(jj-j)*(jj-j);
                    if (d<=r2 && d>=r2l)
                        {
                            dest.SafePixel(ii,jj) = pix;
                        }
                }
        }
}

template <class T>
void AddOvalOutline(YARPImageOf<T>& dest, const T& pix, int i, int j, int h2, int w2)
{
	float x, y;
	for (float th=0; th<2*M_PI; th+=0.01)
        {
            x = j+w2*cos(th);
            y = i+h2*sin(th);
            dest.SafePixel((int)y,(int)x) = pix;
        }
}


template <class T>
void AddRectangle(YARPImageOf<T>& dest, const T& pix, int i, int j, int w, int h)
{
	for (int ii=i-w; ii<=i+w; ii++)
        {
            dest.SafePixel(ii,j-h) = pix;
            dest.SafePixel(ii+1,j-h+1) = pix;
            dest.SafePixel(ii,j+h) = pix;
            dest.SafePixel(ii,j+h-1) = pix;
        }
	for (int jj=j-h; jj<=j+h; jj++)
        {
            dest.SafePixel(i-w,jj) = pix;
            dest.SafePixel(i-w+1,jj) = pix;
            dest.SafePixel(i+w,jj) = pix;
            dest.SafePixel(i+w-1,jj) = pix;
        }
}

//int ApplyLabels(YARPImageOf<int>& src, YARPImageOf<int>& dest);

template <class T>
int ApplyThreshold(YARPImageOf<T>& src, YARPImageOf<T>& dest, 
                   const T& thetalo, const T& thetahi,
                   const T& pix0, const T& pix1)
{
	int h = src.GetHeight();
	int w = src.GetWidth();
	for (int i=0; i<h; i++)
        {
            for (int j=0; j<w; j++)
                {
                    if (src(i,j)>=thetalo && src(i,j)<=thetahi)
                        {
                            dest(i,j) = pix1;
                        }
                    else
                        {
                            dest(i,j) = pix0;
                        }
                }
        }
	return 0;
}

template <class T>
void SetYARPImageOf(YARPImageOf<T>& src, const T& pix)
{
	int h = src.GetHeight();
	int w = src.GetWidth();
	for (int i=0; i<h; i++)
        {
            for (int j=0; j<w; j++)
                {
                    src(i,j) = pix;
                }
        }  
}

#define IMGFOR(img,i,j) for (int i=0; i<(img).GetWidth(); i++) for (int j=0; j<(img).GetHeight(); j++)

#endif

