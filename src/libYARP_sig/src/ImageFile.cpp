/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/ImageFile.h>
#include <yarp/os/Log.h>

#include <cstdio>
#include <cstdlib>

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;


static void warn(const char *message)
{
    fprintf(stderr, "pgm/ppm: Error - %s\n", message);
}


bool file::write(const ImageOf<PixelFloat>& src, const std::string& dest)
{
    FILE *fp = fopen(dest.c_str(),"wb");
    if (fp==nullptr) {
        return false;
    }

    int dims[2] = { src.width(), src.height() };

    size_t bw = 0;
    size_t size_ = sizeof(float);
    size_t count_ = (size_t)(dims[0]*dims[1]);

    if (fwrite(dims,sizeof(dims),1,fp) > 0) {
        bw = fwrite(&src(0,0),size_,count_,fp);
    }

    fclose(fp);
    return (bw > 0);
}


bool file::read(ImageOf<PixelFloat>& dest, const std::string& src)
{
    FILE *fp = fopen(src.c_str(),"rb");
    if (fp==nullptr) {
        return false;
    }

    size_t br = 0;
    size_t size_ = sizeof(float);
    size_t count_ = 0;

    int dims[2];
    if (fread(dims,sizeof(dims),1,fp) > 0) {
        count_ = (size_t)(dims[0]*dims[1]);
        dest.resize(dims[0],dims[1]);
        br = fread(&dest(0,0),size_,count_,fp);
    }

    fclose(fp);
    return (br > 0);
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

            ///fwrite((void *) src, 1, (size_t) (h*w*3), fp);
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
    FILE *fp=nullptr;
    fp = fopen(filename, "rb");

    if(fp==nullptr)
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
    FILE *fp=nullptr;
    fp = fopen(filename, "rb");

    if(fp==nullptr)
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
    FILE *fp=nullptr;
    fp = fopen(filename, "rb");

    if(fp==nullptr)
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


bool file::read(ImageOf<PixelRgb> & dest, const std::string& src)
{
    return ImageReadRGB(dest,src.c_str());
}


bool file::read(ImageOf<PixelBgr> & dest, const std::string& src)
{
    return ImageReadBGR(dest,src.c_str());
}


bool file::read(ImageOf<PixelRgba> & dest, const std::string& src)
{
    ImageOf<PixelRgb> img2;
    bool ok = ImageReadRGB(img2,src.c_str());
    if (ok) {
        dest.copy(img2);
    }
    return ok;
}


bool file::write(const ImageOf<PixelRgb> & src, const std::string& dest)
{
    return ImageWriteRGB(const_cast<ImageOf<PixelRgb> &>(src), dest.c_str());
}


bool file::write(const ImageOf<PixelBgr> & src, const std::string& dest)
{
    return ImageWriteBGR(const_cast<ImageOf<PixelBgr> &>(src), dest.c_str());
}


bool file::write(const ImageOf<PixelRgba> & src, const std::string& dest)
{
    ImageOf<PixelRgb> img2;
    img2.copy(src);
    return ImageWriteRGB(const_cast<ImageOf<PixelRgb> &>(img2), dest.c_str());
}


bool file::read(ImageOf<PixelMono> & dest, const std::string& src)
{
    return ImageReadMono(dest,src.c_str());
}


bool file::write(const ImageOf<PixelMono> & src, const std::string& dest)
{
    return ImageWriteMono(const_cast<ImageOf<PixelMono> &>(src), dest.c_str());
}


bool file::write(const Image& src, const std::string& dest)
{
    int code=src.getPixelCode();
    if (code==VOCAB_PIXEL_MONO)
        return write(static_cast<const ImageOf<PixelMono>&>(src),dest);
    else if (code==VOCAB_PIXEL_MONO_FLOAT)
        return write(static_cast<const ImageOf<PixelFloat>&>(src),dest);
    else if (code==VOCAB_PIXEL_BGR)
        return write(static_cast<const ImageOf<PixelBgr>&>(src),dest);
    else if (code==VOCAB_PIXEL_RGB)
        return write(static_cast<const ImageOf<PixelRgb>&>(src),dest);
    else if (code==VOCAB_PIXEL_RGBA)
        return write(static_cast<const ImageOf<PixelRgba>&>(src),dest);
    else
    {
        ImageOf<PixelRgb> img;
        img.copy(src);
        return write(img,dest);
    }
}

