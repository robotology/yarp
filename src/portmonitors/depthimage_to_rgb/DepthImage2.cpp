/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "DepthImage2.h"

#include <algorithm>
#include <cmath>

#include <yarp/os/LogComponent.h>
#include <yarp/sig/Image.h>

using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(DEPTHIMAGE2,
                   "yarp.carrier.portmonitor.depthimage_to_rgb",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}

void getHeatMapColor(float value, unsigned char& r, unsigned char& g, unsigned char& b)
{
    const int NUM_COLORS = 5;
    static float color[NUM_COLORS][3] = { {0,0,1}, {0,1,0}, {1,1,0}, {1,0,0}, {0,0,0} };

    int idx1;
    int idx2;
    float fractBetween = 0;

    if(value <= 0)      {  idx1 = idx2 = 0;            }
    else if(value >= 1) {  idx1 = idx2 = NUM_COLORS-1; }
    else
    {
       value = value * (NUM_COLORS-1);
       idx1  = floor(value);
       idx2  = idx1+1;
       fractBetween = value - float(idx1);
    }

    r = ((color[idx2][0] - color[idx1][0])*fractBetween + color[idx1][0])*255;
    g = ((color[idx2][1] - color[idx1][1])*fractBetween + color[idx1][1])*255;
    b = ((color[idx2][2] - color[idx1][2])*fractBetween + color[idx1][2])*255;
}

bool DepthImageConverter::create(const yarp::os::Property& options)
{
    min = 0.2;
    max = 10.0;
    outImg.setPixelCode(VOCAB_PIXEL_MONO);
    return true;
}

void DepthImageConverter::destroy()
{
}

bool DepthImageConverter::setparam(const yarp::os::Property& params)
{
    return false;
}

bool DepthImageConverter::getparam(yarp::os::Property& params)
{
    return false;
}

bool DepthImageConverter::accept(yarp::os::Things& thing)
{
    auto* img = thing.cast_as<Image>();
    if(img == nullptr)
    {
        yCError(DEPTHIMAGE2, "Expected type FlexImage but got wrong data type!");
        return false;
    }

    if( img->getPixelCode() == VOCAB_PIXEL_MONO_FLOAT)
    {
        return true;
    }

    yCError(DEPTHIMAGE2,
            "Expected %s, got %s, not doing any conversion!",
            yarp::os::Vocab32::decode(VOCAB_PIXEL_MONO_FLOAT).c_str(),
            yarp::os::Vocab32::decode(img->getPixelCode()).c_str() );
    return false;
}

yarp::os::Things& DepthImageConverter::update(yarp::os::Things& thing)
{
    yarp::sig::Image* img = thing.cast_as<Image>();

    outImg.setPixelCode(VOCAB_PIXEL_RGB);
    outImg.setPixelSize(3);
    outImg.resize(img->width(), img->height());
    outImg.zero();

    auto* inPixels = reinterpret_cast<float *> (img->getRawImage());
    unsigned char *outPixels = outImg.getRawImage();
    for(size_t h=0; h<img->height(); h++)
    {
        for(size_t w=0; w<img->width(); w++)
        {
            float inVal = inPixels[w + (h * img->width())];
            if (inVal != inVal /* NaN */ || inVal < min || inVal > max)
            {
               outPixels[w*3 + (h * (img->width()*3)) + 0] = 0;
               outPixels[w*3 + (h * (img->width()*3)) + 1] = 0;
               outPixels[w*3 + (h * (img->width()*3)) + 2] = 0;
            }
            else
            {
               float dist = inVal  / (max - min);
               unsigned char r,g,b;
               getHeatMapColor (dist, r, g, b);

               outPixels[w*3 + (h * (img->width()*3 )) + 0] = r;
               outPixels[w*3 + (h * (img->width()*3 )) + 1] = g;
               outPixels[w*3 + (h * (img->width()*3 )) + 2] = b;
            }
        }
    }
    th.setPortWriter(&outImg);
    return th;
}
