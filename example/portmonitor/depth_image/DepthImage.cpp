/*
 * Copyright (C) 2016 iCub Facility
 * Authors: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "DepthImage.h"

#include <algorithm>
#include <cstdio>
#include <cmath>

#include <yarp/sig/Image.h>

using namespace yarp::os;
using namespace yarp::sig;


bool DepthImageConverter::create(const yarp::os::Property& options)
{
    min = 0.2;
    max = 10.0;
    inMatrix = NULL;
    outMatrix = NULL;
    outImg.setPixelCode(VOCAB_PIXEL_MONO);
    return true;
}

void DepthImageConverter::destroy(void)
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
    FlexImage* img = thing.cast_as< FlexImage >();
    if(img == NULL) {
        printf("DepthImageConverter: expected type FlexImage but got wrong data type!\n");
        return false;
    }

    if( img->getPixelCode() == VOCAB_PIXEL_MONO_FLOAT)
    {
        return true;
    }

    printf("DepthImageConverter: expected %s, got %s, not doing any convertion!\n", yarp::os::Vocab::decode(VOCAB_PIXEL_MONO_FLOAT).c_str(), yarp::os::Vocab::decode(img->getPixelCode()).c_str() );
    return false;
}

yarp::os::Things& DepthImageConverter::update(yarp::os::Things& thing)
{
    FlexImage* img = thing.cast_as< FlexImage >();
    inMatrix = (float **) img->getRawImage();

    outImg.setPixelCode(VOCAB_PIXEL_MONO);
    outImg.setPixelSize(1);
    outImg.resize(img->width(), img->height());

    outImg.zero();
    float *inPixels = (float *)img->getRawImage();
    unsigned char *pixels = outImg.getRawImage();
    for(int h=0; h<img->height(); h++)
    {
        for(int w=0; w<img->width(); w++)
        {
            float inVal = inPixels[w + (h * img->width())];
            int val = (int) (inVal * 255 / (max - min));
            if(val >= 255)
                val = 250;
            if(val <= 1)
                val = 1;
            pixels[w + (h * (img->width() ))] = (char) val;
            fflush(stdout);
        }
    }
    th.setPortWriter(&outImg);
    return th;
}

