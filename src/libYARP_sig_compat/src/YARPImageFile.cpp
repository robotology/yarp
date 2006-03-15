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
/// $Id: YARPImageFile.cpp,v 1.1 2006-03-15 09:33:51 eshuy Exp $
///
///

///#include <stdio.h>
///#include <stdlib.h>
///#include <ctype.h>

#include <yarp/YARPImageFile.h>
#include <yarp/YARPSimpleOperations.h>

#include <fstream>
using namespace std;

// The PGM/PPM code is old code from a long forgotten source.

static void die(char *message)
{
	ACE_OS::fprintf(stderr, "pgm/ppm: Error - %s\n", message);
	ACE_OS::exit(1);
}

static void warn(char *message)
{
	ACE_OS::fprintf(stderr, "pgm/ppm: Error - %s\n", message);
}

static int SavePGM(char *src, const char *filename, int h, int w)
{
	FILE *fp = ACE_OS::fopen(filename, "wb");
	if (!fp) 
	{
		ACE_OS::printf("cannot open file %s for writing\n", filename);
		return -1;
	}
	else
	{
		const int inc = YARPSimpleOperation::ComputePadding (w, YarpImageAlign) + w;

		ACE_OS::fprintf(fp, "P5\n%d %d\n%d\n", w, h, 255);
		for (int i = 0; i < h; i++)
		{
			ACE_OS::fwrite((void *) src, 1, (size_t) w, fp);
			src += inc;
		}

		ACE_OS::fclose(fp);
	}

	return 0;
}

static int SavePPM(char *src, const char *filename, int h, int w)
{
	FILE *fp = ACE_OS::fopen(filename, "wb");
	if (!fp) 
	{
		ACE_OS::printf("cannot open file %s for writing\n", filename);
		return -1;
	}
	else
	{
		const int inc = YARPSimpleOperation::ComputePadding (w*3, YarpImageAlign) + w * 3;

		ACE_OS::fprintf(fp, "P6\n%d %d\n%d\n", w, h, 255);
		for (int i = 0; i < h; i++)
		{
			ACE_OS::fwrite((void *) src, 1, (size_t) (w*3), fp);
			src += inc;
		}

		///ACE_OS::fwrite((void *) src, 1, (size_t) (h*w*3), fp);
		ACE_OS::fclose(fp);
	}

	return 0;
}


static int ReadHeader(FILE *fp, int *height, int *width, int *color)
{
	char ch;
	int  maxval;

	*color = 0;

	//// LATER: replace fscanf (said to be non portable) with getc style functions.
	if (fscanf(fp, "P%c\n", &ch) != 1 || (ch!='6'&&ch!='5')) 
	//die("file is not in pgm/ppm raw format; cannot read");
	{
		warn("file is not in pgm/ppm raw format; cannot read");
		return -1;
	}

	if (ch=='6') *color = 1;

	// skip comments
	ch = ACE_OS::fgetc(fp);
	while (ch == '#')
	{
		do 
		{
			ch = ACE_OS::fgetc(fp);
		} 
		while (ch != '\n');   
		ch = ACE_OS::fgetc(fp);    
	}
	/*
	while (ch=='\n' || ch=='\r')
	{
	ch = getc(fp);
	}
	ungetc(ch,fp);
	*/

	while(!isdigit(ch))
	{
		ch = ACE_OS::fgetc(fp);
	}

	if (!isdigit(ch)) //die("cannot read header information from pgm/ppm file");
	{
		warn("cannot read header information from pgm/ppm file");
		return -1;
	}
	ungetc(ch, fp);

	/// LATER: not portable?
	fscanf(fp, "%d%d%d", width, height, &maxval);
	ACE_OS::fgetc(fp);
	if (maxval != 255)
	{
		//die("image is not true-color (24 bit); read failed");
		warn("image is not true-color (24 bit); read failed");
		return -1;
	}

	return 0;
}

/// LATER: this is NOT tested.
static int ImageRead(YARPGenericImage& img, const char *filename)
{
	int width, height, color, num, size;

	FILE  *fp = ACE_OS::fopen(filename, "rb");

	if (!fp)    //die("cannot open file for reading");
	{
		warn("cannot open file for reading");
		return -1;
	}

	if (ReadHeader(fp, &height, &width, &color) < 0)
	{
		ACE_OS::fclose (fp);
		return -1;
	}

	if (!color)
		// img.GetID()==YARP_PIXEL_RGB || img.GetID() == YARP_PIXEL_MONO)
	{
		// img.SetID(color?YARP_PIXEL_RGB:YARP_PIXEL_MONO);
		img.SetID(YARP_PIXEL_MONO);
		img.Resize(width,height);
		///ACE_ASSERT(img.GetPadding() == 0);
		ACE_ASSERT(img.GetRawBuffer()!=NULL);

		const int w = img.GetWidth() * img.GetPixelSize();
		const int h = img.GetHeight();
		const int pad = img.GetPadding() + w;
		char *dst = img.GetRawBuffer ();
		size = w * h;

		num = 0;
		for (int i = 0; i < h; i++)
		{
			num += ACE_OS::fread((void *) dst, 1, (size_t) w, fp);
			dst += pad;
		}
	}
	else if (img.GetID()==YARP_PIXEL_RGB)
	{		
		img.SetID(YARP_PIXEL_RGB);
		img.Resize(width,height);
		///ACE_ASSERT(img.GetPadding() == 0);
		ACE_ASSERT(img.GetRawBuffer()!=NULL);

		const int w = img.GetWidth() * img.GetPixelSize();
		const int h = img.GetHeight();
		const int pad = img.GetPadding() + w;
		char *dst = img.GetRawBuffer ();
		size = w * h;

		num = 0;
		for (int i = 0; i < h; i++)
		{
			num += ACE_OS::fread((void *) dst, 1, (size_t) w, fp);
			dst += pad;
		}

	}
	else
	{
		// image is color, nothing was specified, assume BGR
		img.SetID(YARP_PIXEL_BGR);
		img.Resize(width,height);
		///ACE_ASSERT(img.GetPadding() == 0);
		ACE_ASSERT(img.GetRawBuffer()!=NULL);

		const int w = img.GetWidth() * img.GetPixelSize();
		const int h = img.GetHeight();
		const int pad = img.GetPadding() + w;
		size = w * h;

		YARPImageOf<YarpPixelRGB> img2;
		img2.Resize (width,height);
		char *dst = img2.GetRawBuffer ();

		num = 0;
		for (int i = 0; i < h; i++)
		{
			num += ACE_OS::fread((void *) dst, 1, (size_t) w, fp);
			dst += pad;
		}

		img.CastCopy(img2);
	}

	if (num != size) 
	{
		ACE_OS::printf ( "%d versus %d\n", num, size );
		//die("cannot read image data from file");
		warn("cannot read image data from file");
		ACE_OS::fclose (fp);
		return -1;
	}

	ACE_OS::fclose(fp);

	return 0;
}

static int ImageWrite(YARPGenericImage& img, const char *filename)
{
	///ACE_ASSERT(img.GetPadding()==0);
	if (img.GetID()==YARP_PIXEL_MONO)
	{
		SavePGM((char*)img.GetRawBuffer(), filename, img.GetHeight(), img.GetWidth());
	}
	else
	{
		if (img.GetID()==YARP_PIXEL_RGB)
		{
			ACE_ASSERT(img.GetID()==YARP_PIXEL_RGB);
			//      Image<YarpPixelRGB> img2;
			//      img2.ReferOrCopy(img);
			SavePPM((char*)img.GetRawBuffer(),filename,img.GetHeight(),img.GetWidth());
		}
		else
		{
			YARPImageOf<YarpPixelRGB> img2;
			img2.CastCopy(img);
			SavePPM((char*)img2.GetRawBuffer(),filename,img2.GetHeight(), img2.GetWidth());	  
		}
	}

	return 0;
}

int YARPImageFile::Read(const char *src, YARPGenericImage& dest, int format)
{
 if (format!=YARPImageFile::FORMAT_NUMERIC)
    {
      return ImageRead(dest,src);
    }
  int hh = 0, ww = 0;
  {
    ifstream fin(src);
    int blank = 1;
    int curr = 0;
    while (!fin.eof())
      {
	int ch = fin.get();
	//if (ch!='\n') printf("[%c]",ch);
	if (ch==' ' || ch == '\t' || ch == '\r' || ch == '\n' || fin.eof())
	  {
	    if (!blank)
	      {
		if (curr==0)
		  {
		    hh++;
		  }
		curr++;
		if (curr>ww)
		  {
		    ww = curr;
		    //printf("%d\n", ww);
		  }
	      }
	    blank = 1;
	    if (ch=='\n')
	      {
		curr = 0;
	      }
	  }
	else
	  {
	    blank = 0;
	  }
      }
  }
  //printf("yyy dim %d %d\n", hh, ww);
  YARPImageOf<YarpPixelFloat> flt;
  flt.Resize(ww,hh);
  hh = 0; ww = 0;
  {
    char buf[256];
    int idx = 0;
    ifstream fin(src);
    int blank = 1;
    int curr = 0;
    while (!fin.eof())
      {
	int ch = fin.get();
	if (ch==' ' || ch == '\t' || ch == '\r' || ch == '\n' || fin.eof())
	  {
	    if (!blank)
	      {
		if (curr==0)
		  {
		    hh++;
		  }
		curr++;
		if (curr>ww)
		  {
		    ww = curr;
		  }
		buf[idx] = '\0';
		flt(curr-1,hh-1) = atof(buf);
		idx = 0;
	      }
	    blank = 1;
	    if (ch=='\n')
	      {
		curr = 0;
	      }
	  }
	else
	  {
	    buf[idx] = ch;
	    idx++;
	    assert(idx<sizeof(buf));
	    blank = 0;
	  }
      }
  }
    
  dest.CastCopy(flt);

  return 0;

  //return ImageRead(dest,src);
  //return 0;
}


int YARPImageFile::Write(const char *dest, YARPGenericImage& src, int format)
{
 if (format!=YARPImageFile::FORMAT_NUMERIC)
    {
      return ImageWrite(src,dest);
    }
  YARPImageOf<YarpPixelFloat> flt;
  ofstream fout(dest);
  flt.CastCopy(src);
  for (int i=0; i<flt.GetHeight(); i++)
    {
      for (int j=0; j<flt.GetWidth(); j++)
	{
	  char buf[255];
	  sprintf(buf,"%g ", flt(j,i));
	  fout << buf << " ";
	}
      fout << endl;
    }
  return 0;

  //return ImageWrite(src,dest);
  //return 0;
}


