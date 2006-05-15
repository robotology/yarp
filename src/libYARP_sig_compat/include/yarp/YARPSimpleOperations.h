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
/// $Id: YARPSimpleOperations.h,v 1.3 2006-05-15 15:57:59 eshuy Exp $
///
///

//
// YARPSimpleOperations.h
//

#ifndef __YARPSimpleOperationsh__
#define __YARPSimpleOperationsh__

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

//
// add here simple image manipulation stuff. Scaling, sum, diff.
// It's not by any mean exhaustive...

#include <yarp/YARPImage.h>
#include <yarp/YARPFilters.h>

const double __crossLength = 2.0;
const double __crossThickness = 1.0;

/**
 * Legacy namespace for general image operations.
 */
namespace YARPSimpleOperations
{

	void Scale (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelMono>& out, double scale);
	void Threshold (const YARPImageOf<YarpPixelMono> &in, YARPImageOf<YarpPixelMono> &out, unsigned char threshold);
	void Flip (const YARPGenericImage& in, YARPGenericImage& out);
	inline void Fill (YARPGenericImage &img, int value)
	{
		// image type is not checked!
		char *pointer = img.GetRawBuffer();
		ACE_ASSERT (pointer != NULL);
		
		memset (pointer, value, img.GetAllocatedDataSize());
	}

	// decimate or shrinks an image
	// scaleX/scaleY is the actor by which the input image is shrunken (>= 1)
	template <class T>
    void Decimate (const YARPImageOf<T> &in, YARPImageOf<T> &out, double scaleX, double scaleY, int interpolate = IPL_INTER_NN)
    {
        ACE_ASSERT (in.GetIplPointer() != NULL && out.GetIplPointer() != NULL);
        ACE_ASSERT ( (scaleX >= 1) && (scaleY >= 1) );
        ACE_ASSERT (in.GetWidth() == (int ) out.GetWidth()*scaleX);
        ACE_ASSERT (in.GetHeight() == (int ) out.GetHeight()*scaleY);
			
        iplDecimate(in, out, 1, (int)scaleX, 1, (int)scaleY, interpolate);
    }

	template <class T>
    void DrawLine (YARPImageOf<T>& dest, int xstart, int ystart, int xend, int yend, const T& pixel)
    {
        const int width = dest.GetWidth();
        const int height = dest.GetHeight();

        for(int i = 0; i < 2; i++)
			{
				if(xstart +i < width)
                    {
                        for(int j=0; j<2; j++)
                            if(ystart +j < height)
                                dest.Pixel(xstart +i, ystart +j) = pixel;
                    }
			}

        const int dx = xend - xstart;
        const int dy = yend - ystart;
        int d = 2 * dy - dx;
        const int incrE = 2 * dy;
        const int incrNE = 2 * (dy - dx);

        while(xstart < xend)
			{
				if (d <= 0)
                    {
                        d += incrE;
                        xstart++;
                    }
				else
                    {
                        d += incrNE;
                        xstart++;
                        ystart++;
                    }
				dest.Pixel(xstart, ystart) = pixel;
			} 
    }

	template <class T>
    void  DrawCross(YARPImageOf<T> &img, double dx, double dy, const T &pixel, int length = 2, int thick = 1)
	{
		int x = (int) (dx + 0.5);
		int y = (int) (dy + 0.5);

		int i,j,t;
		
		for(t = -thick; t <= thick; t++)
            {
                for(i = -length; i <= length; i++)
                    img.SafePixel(x+i,y+t) = pixel;
                for (j = -length; j <= length; j++)
                    img.SafePixel(x+t,y+j) = pixel;
            }
	}
	
	inline int ComputePadding (int linesize, int align)
	{
		int rem = linesize % align;
		return (rem != 0) ? (align - rem) : rem;
	}
};

// keep backward compatibility with old code (YARPSimpleOperations was previously misspelled)
#define YARPSimpleOperation YARPSimpleOperations

#endif
