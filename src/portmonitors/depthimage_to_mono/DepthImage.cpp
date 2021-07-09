/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "DepthImage.h"

#include <algorithm>
#include <cmath>

#include <yarp/os/LogComponent.h>
#include <yarp/sig/Image.h>

using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(DEPTHIMAGE,
                   "yarp.carrier.portmonitor.depthimage_to_mono",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}

bool DepthImageConverter::create(const yarp::os::Property& options)
{
    min = 0.2;
    max = 10.0;
    inMatrix = nullptr;
    outMatrix = nullptr;
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
    if(img == nullptr) {
        yCError(DEPTHIMAGE, "Expected type FlexImage but got wrong data type!");
        return false;
    }

    if( img->getPixelCode() == VOCAB_PIXEL_MONO_FLOAT)
    {
        return true;
    }

    yCError(DEPTHIMAGE,
            "Expected %s, got %s, not doing any conversion!",
            yarp::os::Vocab32::decode(VOCAB_PIXEL_MONO_FLOAT).c_str(),
            yarp::os::Vocab32::decode(img->getPixelCode()).c_str() );
    return false;
}

yarp::os::Things& DepthImageConverter::update(yarp::os::Things& thing)
{
    auto* img = thing.cast_as<Image>();
    inMatrix = reinterpret_cast<float **> (img->getRawImage());

    outImg.setPixelCode(VOCAB_PIXEL_MONO);
    outImg.setPixelSize(1);
    outImg.resize(img->width(), img->height());

    outImg.zero();
    auto* inPixels = reinterpret_cast<float *> (img->getRawImage());
    unsigned char *pixels = outImg.getRawImage();
    for(size_t h=0; h<img->height(); h++)
    {
        for(size_t w=0; w<img->width(); w++)
        {
            float inVal = inPixels[w + (h * img->width())];
            if (inVal != inVal /* NaN */ || inVal < min || inVal > max) {
                pixels[w + (h * (img->width() ))] = 0;
            } else {
                int val = (int) (255.0 - (inVal * 255.0 / (max - min)));
                if(val >= 255)
                    val = 255;
                if(val <= 0)
                    val = 0;
                pixels[w + (h * (img->width() ))] = (char) val;
            }
        }
    }
    th.setPortWriter(&outImg);
    return th;
}
