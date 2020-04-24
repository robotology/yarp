/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/ImageFile.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>

#if YARP_HAS_JPEG_C
#include "jpeglib.h"
#endif

#if defined (YARP_HAS_PNG)
#include <png.h>
#endif

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// private read methods for PNG Files
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool ImageReadRGB_PNG(ImageOf<PixelRgb>& img, const char* filename)
{
#if defined (YARP_HAS_PNG)
    FILE* fp = fopen(filename, "rb");

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
    {
        yError() << "PNG internal error";
        return false;
    }

    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        yError() << "PNG internal error";
        return false;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        yError() << "PNG internal error";
        return false;
    }

    png_init_io(png, fp);

    png_read_info(png, info);

    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++)
    {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
    }

    png_read_image(png, row_pointers);
    fclose(fp);

    img.resize(width,height);
    for (int y = 0; y < height; y++)
    {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; x++)
            {
                png_bytep px = &(row[x * 4]);
                unsigned char* address = img.getPixelAddress(x,y);
                address[0] = px[0];
                address[1] = px[1];
                address[2] = px[2];
            }
    }

    png_destroy_read_struct(&png, &info, NULL);
    for (int y = 0; y < height; y++)
    {
        free(row_pointers[y]);
    }
    free(row_pointers);
    return true;
#else
    yError() << "PNG library not available/not found";
    return false;
#endif
}

static bool ImageReadFloat_PNG(ImageOf<PixelFloat>& dest, const std::string& filename)
{
#if defined (YARP_HAS_PNG)
    yError() << "Not yet implemented";
    return false;
#else
    yError() << "PNG library not available/not found";
    return false;
#endif
}

static bool ImageReadBGR_PNG(ImageOf<PixelBgr>& img, const char* filename)
{
#if defined (YARP_HAS_PNG)
    yError() << "Not yet implemented";
    return false;
#else
    yError() << "PNG library not available/not found";
    return false;
#endif
}

static bool ImageReadMono_PNG(ImageOf<PixelMono>& img, const char* filename)
{
#if defined (YARP_HAS_PNG)
    yError() << "Not yet implemented";
    return false;
#else
    yError() << "PNG library not available/not found";
    return false;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// private read methods for PGM/PPM Files
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool ReadHeader_PxM(FILE *fp, int *height, int *width, int *color)
{
    char ch;
    int  maxval;

    *color = 0;

    //// LATER: replace fscanf (said to be non portable) with getc style functions.
    if (fscanf(fp, "P%c\n", &ch) != 1 || (ch!='6'&&ch!='5'))
    {
        yWarning("file is not in pgm/ppm raw format; cannot read");
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
        yWarning("image is not true-color (24 bit); read failed");
        return false;
    }

    return true;
}


static bool ImageReadRGB_PxM(ImageOf<PixelRgb> &img, const char *filename)
{
    int width, height, color, num;
    FILE *fp=nullptr;
    fp = fopen(filename, "rb");

    if(fp==nullptr)
    {
        yError("Error opening %s, check if file exists.\n", filename);
        return false;
    }

    if (!ReadHeader_PxM(fp, &height, &width, &color))
    {
        fclose (fp);
        yError("Error reading header, is file a valid ppm/pgm?\n");
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

static bool ImageReadFloat_PxM(ImageOf<PixelFloat>& dest, const std::string& filename)
{
    FILE *fp = fopen(filename.c_str(), "rb");
    if (fp == nullptr) {
        return false;
    }

    size_t br = 0;
    size_t size_ = sizeof(float);
    size_t count_ = 0;

    size_t dims[2];
    if (fread(dims, sizeof(dims), 1, fp) > 0)
    {
        count_ = (size_t)(dims[0] * dims[1]);
        dest.resize(dims[0], dims[1]);
        br = fread(&dest(0, 0), size_, count_, fp);
    }

    fclose(fp);
    return (br > 0);
}

static bool ImageReadBGR_PxM(ImageOf<PixelBgr> &img, const char *filename)
{
    int width, height, color, num;
    FILE *fp=nullptr;
    fp = fopen(filename, "rb");

    if(fp==nullptr)
    {
        yError("Error opening %s, check if file exists.\n", filename);
        return false;
    }

    if (!ReadHeader_PxM(fp, &height, &width, &color))
    {
        fclose (fp);
        yError("Error reading header, is file a valid ppm/pgm?\n");
        return false;
    }

    if (!color)
    {
        fclose(fp);
        yError("File is grayscale, conversion not yet supported\n");
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


static bool ImageReadMono_PxM(ImageOf<PixelMono> &img, const char *filename)
{
    int width, height, color, num;
    FILE *fp=nullptr;
    fp = fopen(filename, "rb");

    if(fp==nullptr)
    {
        yError("Error opening %s, check if file exists.\n", filename);
        return false;
    }

    if (!ReadHeader_PxM(fp, &height, &width, &color))
    {
        fclose (fp);
        yError("Error reading header, is file a valid ppm/pgm?\n");
        return false;
    }

    if (color)
    {
        fclose(fp);
        yError("File is color, conversion not yet supported\n");
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// private write methods
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined (YARP_HAS_PNG)
static bool SavePNG(char *src, const char *filename, size_t h, size_t w, size_t rowSize, png_byte color_type, png_byte bit_depth)
{
    // create file
    if (src == nullptr)
    {
        yError("[write_png_file] Cannot write to file a nullptr image");
        return false;
    }

    if (filename == nullptr)
    {
        yError("[write_png_file] Filename is nullptr");
        return false;
    }

    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        yError("[write_png_file] File %s could not be opened for writing", filename);
        return false;
    }

    // initialize stuff
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        yError("[write_png_file] png_create_write_struct failed");
        fclose(fp);
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        yError("[write_png_file] png_create_info_struct failed");
        fclose(fp);
        return false;
    }

    png_bytep * row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * h);
    for (size_t y = 0; y < h; y++)
    {
        row_pointers[y] = (png_bytep)(src)+y*w;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        yError("[write_png_file] Error during init_io");
        free(row_pointers);
        fclose(fp);
        return false;
    }
    png_init_io(png_ptr, fp);


    // write header
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        yError("[write_png_file] Error during writing header");
        free(row_pointers);
        return false;
    }
    png_set_IHDR(png_ptr, info_ptr, w, h,
        bit_depth, color_type, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);


    // write bytes
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        yError("[write_png_file] Error during writing bytes");
        free(row_pointers);
        fclose(fp);
        return false;
    }
    png_write_image(png_ptr, row_pointers);


    // end write
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        yError("[write_png_file] Error during end of write");
        free(row_pointers);
        fclose(fp);
        return false;
    }
    png_write_end(png_ptr, NULL);

    // cleanup heap allocation
    free(row_pointers);

    fclose(fp);
    return true;
}
#endif

static bool SaveJPG(char *src, const char *filename, int h, int w, int rowSize)
{
#if YARP_HAS_JPEG_C
    int quality = 100;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * outfile;
    JSAMPROW row_pointer[1];
    int row_stride;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    if ((outfile = fopen(filename, "wb")) == nullptr)
    {
        yError("can't write file: %s\n", filename);
        return false;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = w;
    cinfo.image_height = h;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    row_stride = w * 3;

    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = (JSAMPROW)&src[cinfo.next_scanline * row_stride];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
    return true;
#else
    yError() << "libjpeg not installed";
    return false;
#endif
}

static bool SavePGM(char *src, const char *filename, int h, int w, int rowSize)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        yError("cannot open file %s for writing\n", filename);
        return false;
    }
    else
    {
        const int inc = rowSize; ////YARPSimpleOperation::ComputePadding (w, YarpImageAlign) + w;

        fprintf(fp, "P5\n%d %d\n%d\n", w, h, 255);
        for (int i = 0; i < h; i++)
        {
            fwrite((void *)src, 1, (size_t)w, fp);
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
        yError("cannot open file %s for writing\n", filename);
        return false;
    }
    else
    {
        const int inc = rowSize;//YARPSimpleOperation::ComputePadding (w*3, YarpImageAlign) + w * 3;

        fprintf(fp, "P6\n%d %d\n%d\n", w, h, 255);
        for (int i = 0; i < h; i++)
        {
            fwrite((void *)src, 1, (size_t)(w * 3), fp);
            src += inc;
        }

        ///fwrite((void *) src, 1, (size_t) (h*w*3), fp);
        fclose(fp);
    }

    return true;
}

static bool ImageWriteJPG(ImageOf<PixelRgb>& img, const char *filename)
{
    return SaveJPG((char*)img.getRawImage(), filename, img.height(), img.width(), img.getRowSize());
}

static bool ImageWritePNG(ImageOf<PixelRgb>& img, const char *filename)
{
#if defined (YARP_HAS_PNG)
    return SavePNG((char*)img.getRawImage(), filename, img.height(), img.width(), img.getRowSize(), PNG_COLOR_TYPE_RGB, 24);
#else
    yError() << "YARP was not built with png support";
    return false;
#endif
}

static bool ImageWritePNG(ImageOf<PixelMono>& img, const char *filename)
{
#if defined (YARP_HAS_PNG)
    return SavePNG((char*)img.getRawImage(), filename, img.height(), img.width(), img.getRowSize(), PNG_COLOR_TYPE_GRAY, 8);
#else
    yError() << "YARP was not built with png support";
    return false;
#endif
}

static bool ImageWriteRGB(ImageOf<PixelRgb>& img, const char *filename)
{
    return SavePPM((char*)img.getRawImage(),filename,img.height(),img.width(),img.getRowSize());
}

static bool ImageWriteMono(ImageOf<PixelMono>& img, const char *filename)
{
    return SavePGM((char*)img.getRawImage(), filename, img.height(), img.width(), img.getRowSize());
}

static bool ImageWriteFloat(ImageOf<PixelFloat>& img, const char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (fp == nullptr)
    {
        return false;
    }

    size_t dims[2] = { img.width(), img.height() };

    size_t bw = 0;
    size_t size_ = sizeof(float);
    auto count_ = (size_t)(dims[0] * dims[1]);

    if (fwrite(dims, sizeof(dims), 1, fp) > 0) {
        bw = fwrite(&img(0, 0), size_, count_, fp);
    }

    fclose(fp);
    return (bw > 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// public read methods
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool file::read(ImageOf<PixelRgb> & dest, const std::string& src, image_fileformat format)
{
    const char* file_ext = strrchr(src.c_str(), '.');
    if (strcmp(file_ext, ".pgm")==0 ||
        strcmp(file_ext, ".ppm")==0 ||
        format == FORMAT_PGM ||
        format == FORMAT_PPM)
    {
        return ImageReadRGB_PxM(dest,src.c_str());
    }
    else if(strcmp(file_ext, ".png")==0 ||
            format == FORMAT_PNG)
    {
        return ImageReadRGB_PNG(dest, src.c_str());
    }
    else if(strcmp(file_ext, ".jpg") == 0 ||
            strcmp(file_ext, ".jpeg") == 0 ||
            format == FORMAT_JPG)
    {
        yError() << "jpeg not yet implemented";
        return false;
    }
    yError() << "unsupported file format";
    return false;
}


bool file::read(ImageOf<PixelBgr> & dest, const std::string& src, image_fileformat format)
{
    const char* file_ext = strrchr(src.c_str(), '.');
    if (strcmp(file_ext, ".pgm") == 0 ||
        strcmp(file_ext, ".ppm") == 0 ||
        format == FORMAT_PGM ||
        format == FORMAT_PPM)
    {
        return ImageReadBGR_PxM(dest, src.c_str());
    }
    else if (strcmp(file_ext, ".png") == 0 ||
        format == FORMAT_PNG)
    {
        return ImageReadBGR_PNG(dest, src.c_str());
    }
    else if (strcmp(file_ext, ".jpg") == 0 ||
        strcmp(file_ext, ".jpeg") == 0 ||
        format == FORMAT_JPG)
    {
        yError() << "jpeg not yet implemented";
        return false;
    }
    yError() << "unsupported file format";
    return false;
}


bool file::read(ImageOf<PixelRgba> & dest, const std::string& src, image_fileformat format)
{
    const char* file_ext = strrchr(src.c_str(), '.');
    if (strcmp(file_ext, ".pgm") == 0 ||
        strcmp(file_ext, ".ppm") == 0 ||
        format == FORMAT_PGM ||
        format == FORMAT_PPM)
    {
        ImageOf<PixelRgb> img2;
        bool ok = ImageReadRGB_PxM(img2, src.c_str());
        if (ok)
        {
            dest.copy(img2);
        }
        return ok;
    }
    else if (strcmp(file_ext, ".png") == 0 ||
             format == FORMAT_PNG)
    {
        ImageOf<PixelRgb> img2;
        bool ok = ImageReadRGB_PNG(img2, src.c_str());
        if (ok)
        {
            dest.copy(img2);
        }
        return ok;
    }
    else if (strcmp(file_ext, ".jpg") == 0 ||
        strcmp(file_ext, ".jpeg") == 0 ||
        format == FORMAT_JPG)
    {
        yError() << "jpeg not yet implemented";
        return false;
    }
    yError() << "unsupported file format";
    return false;
}

bool file::read(ImageOf<PixelMono> & dest, const std::string& src, image_fileformat format)
{
    const char* file_ext = strrchr(src.c_str(), '.');
    if (strcmp(file_ext, ".pgm") == 0 ||
        strcmp(file_ext, ".ppm") == 0 ||
        format == FORMAT_PGM ||
        format == FORMAT_PPM)
    {
        return ImageReadMono_PxM(dest, src.c_str());
    }
    else if (strcmp(file_ext, ".png") == 0 ||
             format == FORMAT_PNG)
    {
        return ImageReadMono_PNG(dest, src.c_str());
    }
    else if (strcmp(file_ext, ".jpg") == 0 ||
        strcmp(file_ext, ".jpeg") == 0 ||
        format == FORMAT_JPG)
    {
        yError() << "jpeg not yet implemented";
        return false;
    }
    yError() << "unsupported file format";
    return false;
}

bool file::read(ImageOf<PixelFloat>& dest, const std::string& src, image_fileformat format)
{
    const char* file_ext = strrchr(src.c_str(), '.');
    if (strcmp(file_ext, ".pgm") == 0 ||
        strcmp(file_ext, ".ppm") == 0 ||
        format == FORMAT_PGM ||
        format == FORMAT_PPM)
    {
        return ImageReadFloat_PxM(dest, src);
    }
    else if (strcmp(file_ext, ".png") == 0 ||
             format == FORMAT_PNG)
    {
        return ImageReadFloat_PNG(dest, src);
    }
    else if (strcmp(file_ext, ".jpg") == 0 ||
        strcmp(file_ext, ".jpeg") == 0 ||
        format == FORMAT_JPG)
    {
        yError() << "jpeg not yet implemented";
        return false;
    }
    yError() << "unsupported file format";
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////write methods
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool file::write(const ImageOf<PixelRgb> & src, const std::string& dest, image_fileformat format)
{
    if (format == FORMAT_PPM)
    {
        return ImageWriteRGB(const_cast<ImageOf<PixelRgb> &>(src), dest.c_str());
    }
    else  if (format == FORMAT_JPG)
    {
        return ImageWriteJPG(const_cast<ImageOf<PixelRgb> &>(src), dest.c_str());
    }
    else  if (format == FORMAT_PNG)
    {
        return ImageWritePNG(const_cast<ImageOf<PixelRgb> &>(src), dest.c_str());
    }
    else
    {
        yError() << "Invalid format, operation not supported";
        return false;
    }
}

bool file::write(const ImageOf<PixelBgr> & src, const std::string& dest, image_fileformat format)
{
    ImageOf<PixelRgb> imgRGB;
    imgRGB.copy(src);
    if (format == FORMAT_PPM)
    {
        return ImageWriteRGB(const_cast<ImageOf<PixelRgb> &>(imgRGB), dest.c_str());
    }
    else  if (format == FORMAT_JPG)
    {
        return ImageWriteJPG(const_cast<ImageOf<PixelRgb> &>(imgRGB), dest.c_str());
    }
    else  if (format == FORMAT_PNG)
    {
        return ImageWritePNG(const_cast<ImageOf<PixelRgb> &>(imgRGB), dest.c_str());
    }
    else
    {
        yError() << "Invalid format, operation not supported";
        return false;
    }
}


bool file::write(const ImageOf<PixelRgba> & src, const std::string& dest, image_fileformat format)
{
    ImageOf<PixelRgb> imgRGB;
    imgRGB.copy(src);
    if (format == FORMAT_PPM)
    {
        return ImageWriteRGB(const_cast<ImageOf<PixelRgb> &>(imgRGB), dest.c_str());
    }
    else  if (format == FORMAT_JPG)
    {
        return ImageWriteJPG(const_cast<ImageOf<PixelRgb> &>(imgRGB), dest.c_str());
    }
    else  if (format == FORMAT_PNG)
    {
        return ImageWritePNG(const_cast<ImageOf<PixelRgb> &>(imgRGB), dest.c_str());
    }
    else
    {
        yError() << "Invalid format, operation not supported";
        return false;
    }
}


bool file::write(const ImageOf<PixelMono> & src, const std::string& dest, image_fileformat format)
{
    if (format == FORMAT_PGM)
    {
        return ImageWriteMono(const_cast<ImageOf<PixelMono> &>(src), dest.c_str());
    }
    else  if (format == FORMAT_PNG)
    {
        return ImageWritePNG(const_cast<ImageOf<PixelMono> &>(src), dest.c_str());
    }
    else
    {
        yError() << "Invalid format, operation not supported";
        return false;
    }
}

bool file::write(const ImageOf<PixelFloat>& src, const std::string& dest, image_fileformat format)
{
    if (format == FORMAT_NUMERIC)
    {
        return ImageWriteFloat(const_cast<ImageOf<PixelFloat> &>(src), dest.c_str());
    }
    else
    {
        yError() << "Invalid format, operation not supported";
        return false;
    }
}

bool file::write(const Image& src, const std::string& dest, image_fileformat format)
{
    int code=src.getPixelCode();
    if (code == VOCAB_PIXEL_MONO)
    {
        return write(static_cast<const ImageOf<PixelMono>&>(src), dest, format);
    }
    else if (code == VOCAB_PIXEL_MONO_FLOAT)
    {
        return write(static_cast<const ImageOf<PixelFloat>&>(src), dest, format);
    }
    else if (code == VOCAB_PIXEL_BGR)
    {
        return write(static_cast<const ImageOf<PixelBgr>&>(src), dest, format);
    }
    else if (code == VOCAB_PIXEL_RGB)
    {
        return write(static_cast<const ImageOf<PixelRgb>&>(src), dest, format);
    }
    else if (code == VOCAB_PIXEL_RGBA)
    {
        return write(static_cast<const ImageOf<PixelRgba>&>(src), dest, format);
    }
    else
    {
        ImageOf<PixelRgb> img;
        img.copy(src);
        return write(img,dest);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
