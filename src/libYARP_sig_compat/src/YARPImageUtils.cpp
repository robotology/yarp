// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta, Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

///
/// $Id: YARPImageUtils.cpp,v 1.3 2006-10-24 16:43:51 eshuy Exp $
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
