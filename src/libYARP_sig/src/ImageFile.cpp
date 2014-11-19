// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Lorenzo Natale, Giorgio Metta and Paul Fitzpatrick.
* Copyright (C) 2006 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

///
/// $Id: ImageFile.cpp,v 1.20 2007-12-01 18:34:12 eshuy Exp $
///
///

#include <yarp/sig/ImageFile.h>
#include <yarp/os/Log.h>

#include <stdio.h>

using namespace yarp::os;
using namespace yarp::sig;

static void warn(const char *message)
{
	fprintf(stderr, "pgm/ppm: Error - %s\n", message);
}


bool file::write(const ImageOf<PixelFloat>& src, const ConstString& dest) {
	FILE *fp = fopen(dest.c_str(), "w");
    if (fp==NULL) {
        return false;
    }
    
    for (int i=0; i<src.height(); i++) {
        for (int j=0; j<src.width(); j++) {
            fprintf(fp,"%g ", src(j,i));
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
    return false;
}

bool file::read(ImageOf<PixelFloat>& dest, const ConstString& src) {
    int hh = 0, ww = 0;

	FILE *fp = fopen(src.c_str(), "r");
    if (fp==NULL) {
        return false;
    }
    int blank = 1;
    int curr = 0;
    while (!feof(fp)) {
        int ch = fgetc(fp);
        if (ch==' ' || ch == '\t' || ch == '\r' ||ch == '\n'|| feof(fp)){
            if (!blank) { 
                if (curr==0) {
                    hh++;
                }
                curr++;
                if (curr>ww) {
                    ww = curr;
                }
            }
            blank = 1;
            if (ch=='\n') {
                curr = 0;
            }
        } else {
            blank = 0;
        }
    }
    fclose(fp);
	fp = fopen(src.c_str(), "rb");
    if (fp==NULL) {
        return false;
    }
    dest.resize(ww,hh);
    hh = 0; ww = 0;
    {
        char buf[256];
        int idx = 0;
        int blank = 1;
        int curr = 0;
        while (!feof(fp)) {
            int ch = fgetc(fp);
            if (ch==' ' || ch == '\t' ||ch == '\r'||ch == '\n' || feof(fp)){
                if (!blank) {
                    if (curr==0) {
                        hh++;
                    }
                    curr++;
                    if (curr>ww) {
                        ww = curr;
                    }
                    buf[idx] = '\0';
                    dest(curr-1,hh-1) = float(atof(buf));
                    idx = 0;
                }
                blank = 1;
                if (ch=='\n') {
                    curr = 0;
                }
            } else {
                buf[idx] = ch;
                idx++;
                yAssert(((unsigned int)idx)<sizeof(buf));
                blank = 0;
            }
        }
    }
    fclose(fp);
	return true;
}

static bool SavePGM(char *src, const char *filename, int h, int w, int rowSize)
{
	FILE *fp = fopen(filename, "wb");
	if (!fp) 
        {
            printf("cannot open file %s for writing\n", filename);
            return false;
        }
	else
        {
            const int inc = rowSize; ////YARPSimpleOperation::ComputePadding (w, YarpImageAlign) + w;

            fprintf(fp, "P5\n%d %d\n%d\n", w, h, 255);
            for (int i = 0; i < h; i++)
                {
                    fwrite((void *) src, 1, (size_t) w, fp);
                    src += inc;
                }

            fclose(fp);
        }

	return true;
}

static bool SavePPM(char *src, const char *filename, int h, int w, int rowSize)
{
	FILE *fp = fopen(filename, "wb");
	if (!fp) 
        {
            printf("cannot open file %s for writing\n", filename);
            return false;
        }
	else
        {
            const int inc = rowSize;//YARPSimpleOperation::ComputePadding (w*3, YarpImageAlign) + w * 3;

            fprintf(fp, "P6\n%d %d\n%d\n", w, h, 255);
            for (int i = 0; i < h; i++)
                {
                    fwrite((void *) src, 1, (size_t) (w*3), fp);
                    src += inc;
                }

            ///ACE_OS::fwrite((void *) src, 1, (size_t) (h*w*3), fp);
            fclose(fp);
        }

	return true;
}


static bool ReadHeader(FILE *fp, int *height, int *width, int *color)
{
	char ch;
	int  maxval;

	*color = 0;

	//// LATER: replace fscanf (said to be non portable) with getc style functions.
	if (fscanf(fp, "P%c\n", &ch) != 1 || (ch!='6'&&ch!='5')) 
        //die("file is not in pgm/ppm raw format; cannot read");
        {
            warn("file is not in pgm/ppm raw format; cannot read");
            return false;
        }

	if (ch=='6') *color = 1;

	// skip comments
	ch = fgetc(fp);
	while (ch == '#')
        {
            do 
                {
                    ch = fgetc(fp);
                } 
            while (ch != '\n');   
            ch = fgetc(fp);    
        }
	/*
      while (ch=='\n' || ch=='\r')
      {
      ch = getc(fp);
      }
      ungetc(ch,fp);
	*/

	while(ch<'0'&&ch>'9'&&ch!=-1)
        {
            ch = fgetc(fp);
        }

	if (ch<'0'&&ch>'9')
        {
            warn("cannot read header information from pgm/ppm file");
            return false;
        }
	ungetc(ch, fp);

	/// LATER: not portable?
	int n=fscanf(fp, "%d%d%d", width, height, &maxval);
    if (n!=3)
        return false;

	fgetc(fp);
	if (maxval != 255)
        {
            //die("image is not true-color (24 bit); read failed");
            warn("image is not true-color (24 bit); read failed");
            return false;
        }

	return true;
}

static bool ImageReadRGB(ImageOf<PixelRgb> &img, const char *filename)
{
   	int width, height, color, num;
    FILE *fp=0;
	fp = fopen(filename, "rb");

    if(fp==0)
        {
            fprintf(stderr, "Error opening %s, check if file exists.\n", filename);
            return false;
        }

	if (!ReadHeader(fp, &height, &width, &color))
        {
            fclose (fp);
            fprintf(stderr, "Error reading header, is file a valid ppm/pgm?\n");
            return false;
        }

    if (!color)
        {
            ImageOf<PixelMono> tmp;
            tmp.resize(width,height);

            const int w = tmp.width() * tmp.getPixelSize();
            const int h = tmp.height();
            const int pad = tmp.getRowSize();
            unsigned char *dst = tmp.getRawImage ();

            num = 0;
            for (int i = 0; i < h; i++)
                {
                    num += (int)fread((void *) dst, 1, (size_t) w, fp);
                    dst += pad;
                }
            fclose(fp);
            img.copy(tmp);
            return true;
        }
	
    img.resize(width,height);

	const int w = img.width() * img.getPixelSize();
	const int h = img.height();
	const int pad = img.getRowSize();
	unsigned char *dst = img.getRawImage ();

	num = 0;
	for (int i = 0; i < h; i++)
        {
            num += (int)fread((void *) dst, 1, (size_t) w, fp);
            dst += pad;
        }
    
	fclose(fp);

	return true;
}

static bool ImageReadBGR(ImageOf<PixelBgr> &img, const char *filename)
{
   	int width, height, color, num;
    FILE *fp=0;
	fp = fopen(filename, "rb");

    if(fp==0)
        {
            fprintf(stderr, "Error opening %s, check if file exists.\n", filename);
            return false;
        }

	if (!ReadHeader(fp, &height, &width, &color))
        {
            fclose (fp);
            fprintf(stderr, "Error reading header, is file a valid ppm/pgm?\n");
            return false;
        }
    if (!color)
        {
            fclose(fp);
            fprintf(stderr, "File is grayscale, conversion not yet supported\n");
            return false;
        }
	
    ImageOf<PixelRgb> tmpImg;
    tmpImg.resize(width, height);

	const int w = tmpImg.width() * img.getPixelSize();
	const int h = tmpImg.height();
	const int pad = tmpImg.getRowSize();
	unsigned char *dst = tmpImg.getRawImage ();

	num = 0;
	for (int i = 0; i < h; i++)
        {
            num += (int)fread((void *) dst, 1, (size_t) w, fp);
            dst += pad;
        }

	fclose(fp);
	
	return img.copy(tmpImg);
}

static bool ImageReadMono(ImageOf<PixelMono> &img, const char *filename)
{
   	int width, height, color, num;
    FILE *fp=0;
	fp = fopen(filename, "rb");

    if(fp==0)
        {
            fprintf(stderr, "Error opening %s, check if file exists.\n", filename);
            return false;
        }

	if (!ReadHeader(fp, &height, &width, &color))
        {
            fclose (fp);
            fprintf(stderr, "Error reading header, is file a valid ppm/pgm?\n");
            return false;
        }
    if (color)
        {
            fclose(fp);
            fprintf(stderr, "File is color, conversion not yet supported\n");
            return false;
        }
	
    img.resize(width,height);

	const int w = img.width() * img.getPixelSize();
	const int h = img.height();
	const int pad = img.getRowSize();
	unsigned char *dst = img.getRawImage ();

	num = 0;
	for (int i = 0; i < h; i++)
        {
            num += (int)fread((void *) dst, 1, (size_t) w, fp);
            dst += pad;
        }

	fclose(fp);

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
	return SavePPM((char*)img2.getRawImage(),filename,img2.height(), img2.width(), img2.getRowSize());	  
}

static bool ImageWriteMono(ImageOf<PixelMono>& img, const char *filename)
{
    return SavePGM((char*)img.getRawImage(), filename, img.height(), img.width(), img.getRowSize());
}

bool file::read(ImageOf<PixelRgb> & dest, const ConstString& src)
{
    return ImageReadRGB(dest,src.c_str());
}

bool file::read(ImageOf<PixelBgr> & dest, const ConstString& src)
{
    return ImageReadBGR(dest,src.c_str());
}

bool file::read(ImageOf<PixelRgba> & dest, const ConstString& src)
{
	ImageOf<PixelRgb> img2;
    bool ok = ImageReadRGB(img2,src.c_str());
    if (ok) {
        dest.copy(img2);
    }
    return ok;
}

bool file::write(const ImageOf<PixelRgb> & src, const ConstString& dest)
{
    return ImageWriteRGB(const_cast<ImageOf<PixelRgb> &>(src), dest.c_str());
}

bool file::write(const ImageOf<PixelBgr> & src, const ConstString& dest)
{
    return ImageWriteBGR(const_cast<ImageOf<PixelBgr> &>(src), dest.c_str());
}

bool file::write(const ImageOf<PixelRgba> & src, const ConstString& dest)
{
	ImageOf<PixelRgb> img2;
	img2.copy(src);
    return ImageWriteRGB(const_cast<ImageOf<PixelRgb> &>(img2), dest.c_str());
}

bool file::read(ImageOf<PixelMono> & dest, const ConstString& src)
{
    return ImageReadMono(dest,src.c_str());
}

bool file::write(const ImageOf<PixelMono> & src, const ConstString& dest)
{
    return ImageWriteMono(const_cast<ImageOf<PixelMono> &>(src), dest.c_str());
}


bool file::write(const Image& src, const ConstString& dest)
{
    ImageOf<PixelRgb> img;
    img.copy(src);
    return write(img,dest);
}

