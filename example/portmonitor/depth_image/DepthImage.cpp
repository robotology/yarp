// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2016 iCub Facility
 * Authors: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <yarp/sig/Image.h>
#include <yarp/os/SharedLibraryClass.h>
#include "DepthImage.h"

using namespace yarp::os;
using namespace yarp::sig;

YARP_DEFINE_SHARED_SUBCLASS(MonitorObject_there, DepthImageConverter, MonitorObject);

bool DepthImageConverter::create(const yarp::os::Property& options)
{
    min = 0.2;
    max = 10.0;
    inMatrix = NULL;
    outMatrix = NULL;
    printf("created!\n");
    outImg.setPixelCode(VOCAB_PIXEL_MONO);
    return true;
}

void DepthImageConverter::destroy(void)
{
    printf("destroyed!\n");
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
//         printf("DepthImageConverter: got %s, ok!\n", yarp::os::Vocab::decode(VOCAB_PIXEL_MONO_FLOAT).c_str());
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

//     outMatrix = (unsigned char **) outImg.getRawImage();

    outImg.zero();
    float *inPixels = (float *)img->getRawImage();
    unsigned char *pixels = outImg.getRawImage();
    for(int h=0; h<img->height(); h++)
    {
        for(int w=0; w<img->width(); w++)
        {
            float inVal = inPixels[w + (h * img->width())];
            int val = (int) (inVal * 255 / (max - min));
//             int val = (int) floor(( *(float *)(img->getPixelAddress(w, h)) * 255.0/(max - min)));
            if(val >= 255)
                val = 250;
            if(val <= 1)
                val = 1;
            pixels[w + (h * (img->width() ))] = (char) val;
//             *(outImg.getPixelAddress(w, h)) = h;
//             outMatrix[h][w] = inMatrix[h][w] * 255.0/(max - min);
//             *outImg.getPixelAddress(w, h) = 200; //*(float *)(img->getPixelAddress(w, h)) * 255.0/(max - min);
            if(w==0)
                printf("w: %3d h: %3d f:%4.2f o: %d (%d) - sizeIn %d sizeOut %d \n", w, h, inVal, pixels[w + (h * (img->width() ))], val, img->getPixelSize(), outImg.getPixelSize());
            fflush(stdout);
/*
            if(inMatrix[h][w] < min)
                outMatrix[h][w] = 0;

            if(inMatrix[h][w] > max)
                outMatrix[h][w] = 255;*/

        }
    }

//     printf("ciao\n");
//     fflush(stdout);
//     fflush(stdout);

    th.setPortWriter(&outImg);
//     th.setPortWriter(img);
    return th;
}

