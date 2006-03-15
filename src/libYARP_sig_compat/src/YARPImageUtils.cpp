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
/// $Id: YARPImageUtils.cpp,v 1.1 2006-03-15 09:33:51 eshuy Exp $
///
///

//

#include <yarp/YARPImageUtils.h>

void GetPlane (const YARPGenericImage& in, YARPImageOf<YarpPixelMono>& out, int shift)
{
	ACE_ASSERT (in.GetIplPointer() != NULL && out.GetIplPointer() != NULL);
	ACE_ASSERT (in.GetWidth() == out.GetWidth());
	ACE_ASSERT (in.GetHeight() == out.GetHeight());

	const int width = in.GetWidth();
	const int height = in.GetHeight();

	unsigned char *src = NULL;
	unsigned char *dst = NULL;

	for (int i = 0; i < height; i++)
	{
		src = (unsigned char *)in.GetArray()[i] + shift;
		dst = (unsigned char *)out.GetArray()[i];
		for (int j = 0; j < width; j++)
		{
			*dst++ = *src;
			src += 3;
		}
	}
}

void SetPlane (const YARPImageOf<YarpPixelMono>& in, YARPGenericImage& out, int shift)
{
	ACE_ASSERT (in.GetIplPointer() != NULL && out.GetIplPointer() != NULL);
	ACE_ASSERT (in.GetWidth() == out.GetWidth());
	ACE_ASSERT (in.GetHeight() == out.GetHeight());

	const int width = in.GetWidth();
	const int height = in.GetHeight();

	unsigned char *src = NULL;
	unsigned char *dst = NULL;

	for (int i = 0; i < height; i++)
	{
		src = (unsigned char *)in.GetArray()[i];
		dst = (unsigned char *)out.GetArray()[i] + shift;
		for (int j = 0; j < width; j++)
		{
			*dst = *src++;
			dst += 3;
		}
	}
}

void YARPImageUtils::GetRed (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelMono>& out)
{
	GetPlane (in, out, 0);
}

void YARPImageUtils::GetRed (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelMono>& out)
{
	GetPlane (in, out, 2);
}

void YARPImageUtils::GetGreen (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelMono>& out)
{
	GetPlane (in, out, 1);
}

void YARPImageUtils::GetGreen (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelMono>& out)
{
	GetPlane (in, out, 1);
}

void YARPImageUtils::GetBlue (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelMono>& out)
{
	GetPlane (in, out, 2);
}

void YARPImageUtils::GetBlue (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelMono>& out)
{
	GetPlane (in, out, 0);
}

void YARPImageUtils::GetValue (const YARPImageOf<YarpPixelHSV>& in, YARPImageOf<YarpPixelMono>& out)
{
	GetPlane (in, out, 2);
}

void YARPImageUtils::GetSaturation (const YARPImageOf<YarpPixelHSV>& in, YARPImageOf<YarpPixelMono>& out)
{
	GetPlane (in, out, 1);
}

void YARPImageUtils::GetHue (const YARPImageOf<YarpPixelHSV>& in, YARPImageOf<YarpPixelMono>& out)
{
	GetPlane (in, out, 0);
}

void YARPImageUtils::SetRed (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelRGB>& out)
{
	SetPlane (in, out, 0);
}

void YARPImageUtils::SetRed (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelBGR>& out)
{
	SetPlane (in, out, 2);
}

void YARPImageUtils::SetGreen (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelRGB>& out)
{
	SetPlane (in, out, 1);
}

void YARPImageUtils::SetGreen (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelBGR>& out)
{
	SetPlane (in, out, 1);
}

void YARPImageUtils::SetBlue (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelRGB>& out)
{
	SetPlane (in, out, 2);
}

void YARPImageUtils::SetBlue (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelBGR>& out)
{
	SetPlane (in, out, 0);
}

void YARPImageUtils::PasteInto (const YARPImageOf<YarpPixelMono>& src, int x, int y, int zoom, YARPImageOf<YarpPixelMono>& dst)
{
	char *bs = dst.GetRawBuffer ();

	IplImage *ipl = src.GetIplPointer ();
	const int dh = ipl->height;
	const int dw = ipl->width;
	char *dsY = ipl->imageData;

	int depth = dst.GetPixelSize ();
	ACE_ASSERT (depth == ipl->nChannels);	// same # of chan.

	const int h = dst.GetHeight();
	ACE_ASSERT (h >= dh);			// same height.
    const int w = dst.GetWidth();
	ACE_ASSERT (w >= dw);			// same width.

	const int rem_w = w - dw;

	// crude limit check.
	ACE_ASSERT (dw * zoom + x < w);
	ACE_ASSERT (dh * zoom + y < h);

	if (zoom == 1)
	{
		bs += (y * w);
		for (int i = 0; i < dh; i++)
		{
			memcpy (bs + x, dsY, dw);

			bs += w;
			dsY += dw;
		}
	}
	else
	{
		bs += (y * w);
		for (int i = 0; i < dh; i++)
		{
			char * st_row = bs;
			bs += x;
			for (int j = 0; j < dw; j++)
			{
				for (int k = 0; k < zoom; k++)
				{
					*bs++ = *dsY;
				}
				dsY++;
			}

			for (int k = 1; k < zoom; k++)
				memcpy (st_row + x + w * k, st_row + x, dw * zoom); 

			bs = st_row + w * zoom;
		}
	}
}
