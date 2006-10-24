// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta, Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

///
/// $Id: YARPSimpleOperations.cpp,v 1.3 2006-10-24 16:43:51 eshuy Exp $
///
///

//
// YARPSimpleOperations.cpp
//

#include <yarp/YARPSimpleOperations.h>

void YARPSimpleOperation::Scale (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelMono>& out, double scale)
{
	ACE_ASSERT (in.GetIplPointer() != NULL && out.GetIplPointer() != NULL);
	ACE_ASSERT (in.GetWidth() == out.GetWidth() && in.GetHeight() == out.GetHeight());
	ACE_ASSERT (scale <= 1 && scale >= 0);

	const int csize = out.GetIplPointer()->imageSize;
	char *tmpo = out.GetIplPointer()->imageData;
	char *tmpi = in.GetIplPointer()->imageData;

	// not sure about the correctness of this.
	for (int i = 0; i < csize; i++, tmpo++, tmpi++)
        {
            *tmpo = char(*tmpi * scale);
        }
}

void YARPSimpleOperation::Flip (const YARPGenericImage& in, YARPGenericImage& out)
{
	ACE_ASSERT (in.GetIplPointer() != NULL && out.GetIplPointer() != NULL);
	ACE_ASSERT (in.GetWidth() == out.GetWidth() && in.GetHeight() == out.GetHeight());

	const int w = out.GetAllocatedLineSize();
	const int h = out.GetHeight();
	
	char *tmpo = out.GetIplPointer()->imageData + out.GetAllocatedDataSize() - w;
	char *tmpi = in.GetIplPointer()->imageData;

	int i;
	for (i = 0; i < h; i++, tmpi += w, tmpo -= w)
        {
            memcpy (tmpo, tmpi, w);
        }
}

///
///
/*
  void YARPSimpleOperation::DrawLine (YARPImageOf<YarpPixelRGB>& dest, int xstart, int ystart, int xend, int yend, const YarpPixelRGB& pixel)
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


  void YARPSimpleOperation::DrawLine (YARPImageOf<YarpPixelBGR>& dest, int xstart, int ystart, int xend, int yend, const YarpPixelBGR& pixel)
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


  void YARPSimpleOperation::DrawLine (YARPImageOf<YarpPixelMono>& dest, int xstart, int ystart, int xend, int yend, const YarpPixelMono& pixel)
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

*/
void YARPSimpleOperation::Threshold (const YARPImageOf<YarpPixelMono> &in, YARPImageOf<YarpPixelMono> &out, unsigned char threshold)
{
	iplThreshold(in, out, threshold);
}
