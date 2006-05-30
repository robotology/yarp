///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa, paulfitz#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: ImageFile.cpp,v 1.1 2006-05-30 17:41:42 natta Exp $
///
///

#include "ImageFile.h"
#include <ace/OS.h>
#include <ace/Log_Msg.h>
#include <fstream>

using namespace std;
using namespace yarp::sig;

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

static int SavePGM(char *src, const char *filename, int h, int w, int rowSize)
{
	FILE *fp = ACE_OS::fopen(filename, "wb");
	if (!fp) 
	{
		ACE_OS::printf("cannot open file %s for writing\n", filename);
		return -1;
	}
	else
	{
		const int inc = rowSize; ////YARPSimpleOperation::ComputePadding (w, YarpImageAlign) + w;

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

static int SavePPM(char *src, const char *filename, int h, int w, int rowSize)
{
	FILE *fp = ACE_OS::fopen(filename, "wb");
	if (!fp) 
	{
		ACE_OS::printf("cannot open file %s for writing\n", filename);
		return -1;
	}
	else
	{
		const int inc = rowSize;//YARPSimpleOperation::ComputePadding (w*3, YarpImageAlign) + w * 3;

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

static bool ImageReadRGB(ImageOf<PixelRgb> &img, const char *filename)
{
   	int width, height, color, num, size;
    FILE *fp=0;
	fp = ACE_OS::fopen(filename, "rb");

    if(fp==0)
    {
        ACE_OS::fprintf(stderr, "Error opening %s, check if file exists.\n", filename);
        return false;
    }

	if (ReadHeader(fp, &height, &width, &color) < 0)
	{
		ACE_OS::fclose (fp);
        ACE_OS::fprintf(stderr, "Error reading header, is file a valid ppm/pgm?\n");
		return false;
	}
    if (!color)
    {
        ACE_OS::fclose(fp);
        ACE_OS::fprintf(stderr, "File is grayscale, conversion not yet supported\n");
        return false;
    }
	
    img.resize(width,height);

	const int w = img.width() * img.getPixelSize();
	const int h = img.height();
	const int pad = img.getRowSize();
	char *dst = img.getRawImage ();
	size = w * h;

	num = 0;
	for (int i = 0; i < h; i++)
	{
		num += ACE_OS::fread((void *) dst, 1, (size_t) w, fp);
		dst += pad;
	}
    return true;
}

static bool ImageReadBGR(ImageOf<PixelBgr> &img, const char *filename)
{
   	int width, height, color, num, size;
    FILE *fp=0;
	fp = ACE_OS::fopen(filename, "rb");

    if(fp==0)
    {
        ACE_OS::fprintf(stderr, "Error opening %s, check if file exists.\n", filename);
        return false;
    }

	if (ReadHeader(fp, &height, &width, &color) < 0)
	{
		ACE_OS::fclose (fp);
        ACE_OS::fprintf(stderr, "Error reading header, is file a valid ppm/pgm?\n");
		return false;
	}
    if (!color)
    {
        ACE_OS::fclose(fp);
        ACE_OS::fprintf(stderr, "File is grayscale, conversion not yet supported\n");
        return false;
    }
	
    ImageOf<PixelRgb> tmpImg;
    tmpImg.resize(width, height);

	const int w = tmpImg.width() * img.getPixelSize();
	const int h = tmpImg.height();
	const int pad = tmpImg.getRowSize();
	char *dst = tmpImg.getRawImage ();
	size = w * h;

	num = 0;
	for (int i = 0; i < h; i++)
	{
		num += ACE_OS::fread((void *) dst, 1, (size_t) w, fp);
		dst += pad;
	}

	return img.copy(tmpImg);
}

static bool ImageReadMono(ImageOf<PixelMono> &img, const char *filename)
{
   	int width, height, color, num, size;
    FILE *fp=0;
	fp = ACE_OS::fopen(filename, "rb");

    if(fp==0)
    {
        ACE_OS::fprintf(stderr, "Error opening %s, check if file exists.\n", filename);
        return false;
    }

	if (ReadHeader(fp, &height, &width, &color) < 0)
	{
		ACE_OS::fclose (fp);
        ACE_OS::fprintf(stderr, "Error reading header, is file a valid ppm/pgm?\n");
		return false;
	}
    if (color)
    {
        ACE_OS::fclose(fp);
        ACE_OS::fprintf(stderr, "File is color, conversion not yet supported\n");
        return false;
    }
	
    img.resize(width,height);

	const int w = img.width() * img.getPixelSize();
	const int h = img.height();
	const int pad = img.getRowSize();
	char *dst = img.getRawImage ();
	size = w * h;

	num = 0;
	for (int i = 0; i < h; i++)
	{
		num += ACE_OS::fread((void *) dst, 1, (size_t) w, fp);
		dst += pad;
	}
    return true;
}

static bool ImageWriteRGB(ImageOf<PixelRgb>& img, const char *filename)
{
    return SavePPM((char*)img.getRawImage(),filename,img.height(),img.width(),img.getRowSize());
}

static bool ImageWriteBGR(ImageOf<PixelBgr>& img, const char *filename)
{
	ImageOf<PixelRgb> img2;
	img2.copy(img);
	return SavePPM((char*)img2.getRawImage(),filename,img2.height(), img2.width(), img.getRowSize());	  
}

static int ImageWriteMono(ImageOf<PixelMono>& img, const char *filename)
{
    return SavePGM((char*)img.getRawImage(), filename, img.height(), img.width(), img.getRowSize());
}

int file::read(ImageOf<PixelRgb> & dest, const char *src)
{
   return ImageReadRGB(dest,src);
}

int file::read(ImageOf<PixelBgr> & dest, const char *src)
{
   return ImageReadBGR(dest,src);
}

int file::write(ImageOf<PixelRgb> & src, const char *dest)
{
    return ImageWriteRGB(src, dest);
}

int file::write(ImageOf<PixelBgr> & src, const char *dest)
{
    return ImageWriteBGR(src, dest);
}

int file::read(ImageOf<PixelMono> & dest, const char *src)
{
   return ImageReadMono(dest,src);
}

int file::write(ImageOf<PixelMono> & src, const char *dest)
{
    return ImageWriteMono(src, dest);
}

