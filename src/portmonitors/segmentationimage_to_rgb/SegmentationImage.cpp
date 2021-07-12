/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SegmentationImage.h"

#include <algorithm>
#include <cmath>

#include <yarp/os/LogComponent.h>
#include <yarp/sig/Image.h>

using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(SEGMENTATIONIMAGE,
                   "yarp.carrier.portmonitor.segmentationimage_to_rgb",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}


rgbColor string2color(std::string colorstring)
{
    rgbColor c;
    c.r = strtol(colorstring.substr(0, 2).c_str(), NULL, 16);
    c.g = strtol(colorstring.substr(2, 2).c_str(), NULL, 16);
    c.b = strtol(colorstring.substr(4, 2).c_str(), NULL, 16);
    return c;
}

bool SegmentationImageConverter::create(const yarp::os::Property& options)
{
    outImg.setPixelCode(VOCAB_PIXEL_MONO);

    size_t index = 0;
    colormap[index++] = string2color("000000");
    colormap[index++] = string2color("00FF00");
    colormap[index++] = string2color("0000FF");
    colormap[index++] = string2color("FF0000");
    colormap[index++] = string2color("01FFFE");
    colormap[index++] = string2color("FFA6FE");
    colormap[index++] = string2color("FFDB66");
    colormap[index++] = string2color("006401");
    colormap[index++] = string2color("010067");
    colormap[index++] = string2color("95003A");
    colormap[index++] = string2color("007DB5");
    colormap[index++] = string2color("FF00F6");
    colormap[index++] = string2color("FFEEE8");
    colormap[index++] = string2color("774D00");
    colormap[index++] = string2color("90FB92");
    colormap[index++] = string2color("0076FF");
    colormap[index++] = string2color("D5FF00");
    colormap[index++] = string2color("FF937E");
    colormap[index++] = string2color("6A826C");
    colormap[index++] = string2color("FF029D");
    colormap[index++] = string2color("FE8900");
    colormap[index++] = string2color("7A4782");
    colormap[index++] = string2color("7E2DD2");
    colormap[index++] = string2color("85A900");
    colormap[index++] = string2color("FF0056");
    colormap[index++] = string2color("A42400");
    colormap[index++] = string2color("00AE7E");
    colormap[index++] = string2color("683D3B");
    colormap[index++] = string2color("BDC6FF");
    colormap[index++] = string2color("263400");
    colormap[index++] = string2color("BDD393");
    colormap[index++] = string2color("00B917");
    colormap[index++] = string2color("9E008E");
    colormap[index++] = string2color("001544");
    colormap[index++] = string2color("C28C9F");
    colormap[index++] = string2color("FF74A3");
    colormap[index++] = string2color("01D0FF");
    colormap[index++] = string2color("004754");
    colormap[index++] = string2color("E56FFE");
    colormap[index++] = string2color("788231");
    colormap[index++] = string2color("0E4CA1");
    colormap[index++] = string2color("91D0CB");
    colormap[index++] = string2color("BE9970");
    colormap[index++] = string2color("968AE8");
    colormap[index++] = string2color("BB8800");
    colormap[index++] = string2color("43002C");
    colormap[index++] = string2color("DEFF74");
    colormap[index++] = string2color("00FFC6");
    colormap[index++] = string2color("FFE502");
    colormap[index++] = string2color("620E00");
    colormap[index++] = string2color("008F9C");
    colormap[index++] = string2color("98FF52");
    colormap[index++] = string2color("7544B1");
    colormap[index++] = string2color("B500FF");
    colormap[index++] = string2color("00FF78");
    colormap[index++] = string2color("FF6E41");
    colormap[index++] = string2color("005F39");
    colormap[index++] = string2color("6B6882");
    colormap[index++] = string2color("5FAD4E");
    colormap[index++] = string2color("A75740");
    colormap[index++] = string2color("A5FFD2");
    colormap[index++] = string2color("FFB167");
    colormap[index++] = string2color("009BFF");
    colormap[index++] = string2color("E85EBE");
    max_colors=index;

    return true;
}

void SegmentationImageConverter::destroy()
{
}

bool SegmentationImageConverter::setparam(const yarp::os::Property& params)
{
    return false;
}

bool SegmentationImageConverter::getparam(yarp::os::Property& params)
{
    return false;
}

bool SegmentationImageConverter::accept(yarp::os::Things& thing)
{
    yarp::sig::Image* img = thing.cast_as<Image>();
    if(img == nullptr)
    {
        yCError(SEGMENTATIONIMAGE, "SegmentationImageConverter: expected type FlexImage but got wrong data type!");
        return false;
    }

    if( img->getPixelCode() == VOCAB_PIXEL_MONO || img->getPixelCode() == VOCAB_PIXEL_MONO16)
    {
        return true;
    }

    yCError(SEGMENTATIONIMAGE,
            "SegmentationImageConverter: expected %s or %s, got %s, not doing any conversion!",
            yarp::os::Vocab32::decode(VOCAB_PIXEL_MONO).c_str(),
            yarp::os::Vocab32::decode(VOCAB_PIXEL_MONO16).c_str(),
            yarp::os::Vocab32::decode(img->getPixelCode()).c_str() );
    return false;
}

yarp::os::Things& SegmentationImageConverter::update(yarp::os::Things& thing)
{
    yarp::sig::Image* img = thing.cast_as<Image>();
    int pixcode = img->getPixelCode();

    outImg.setPixelCode(VOCAB_PIXEL_RGB);
    outImg.setPixelSize(3);
    outImg.resize(img->width(), img->height());
    outImg.zero();
    unsigned char* outPixels = outImg.getRawImage();



    for(size_t h=0; h<img->height(); h++)
    {
        for(size_t w=0; w<img->width(); w++)
        {
            int inVal = 0;
            if (pixcode == VOCAB_PIXEL_MONO)
            {
                char* inPixels = reinterpret_cast<char*> (img->getRawImage());
                inVal = (int) inPixels[w + (h * img->width())];
            }
            else if (pixcode == VOCAB_PIXEL_MONO16)
            {
                short int* inPixels = reinterpret_cast<short int*> (img->getRawImage());
                inVal = inPixels[w + (h * img->width())];
            }
            int index = inVal % max_colors;
            outPixels[w*3 + (h * (img->width()*3 )) + 0] = colormap[index].r;
            outPixels[w*3 + (h * (img->width()*3 )) + 1] = colormap[index].g;
            outPixels[w*3 + (h * (img->width()*3 )) + 2] = colormap[index].b;
        }
    }
    th.setPortWriter(&outImg);
    return th;
}
