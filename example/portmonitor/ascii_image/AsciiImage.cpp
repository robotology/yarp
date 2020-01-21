/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <yarp/sig/Image.h>
#include "AsciiImage.h"

constexpr size_t WINDOW_SIZE = 6;

using namespace yarp::os;
using namespace yarp::sig;


static char available[] = {' ', '`', '.', '~', '+', 'I', 'X', 'O', '8', '%', 'W'};

bool AsciiImageMonitorObject::create(const yarp::os::Property& options)
{
   printf("created!\n");
   return true;
}

void AsciiImageMonitorObject::destroy(void)
{
    printf("destroyed!\n");
}

bool AsciiImageMonitorObject::setparam(const yarp::os::Property& params)
{
    return false;
}

bool AsciiImageMonitorObject::getparam(yarp::os::Property& params)
{
    return false;
}

bool AsciiImageMonitorObject::accept(yarp::os::Things& thing)
{
    ImageOf<PixelRgb>* img = thing.cast_as< ImageOf<PixelRgb> >();
    if(img == NULL) {
        printf("AsciiImageMonitorObject: expected type ImageOf<PixelRgb> but got wrong data type!\n");
        return false;
    }

    return true;
}

yarp::os::Things& AsciiImageMonitorObject::update(yarp::os::Things& thing)
{
    ImageOf<PixelRgb>* img = thing.cast_as< ImageOf<PixelRgb> >();
    bt.clear();
    int col = floor(img->width()  / WINDOW_SIZE) + 1;
    int row = floor(img->height() / WINDOW_SIZE) + 1;

    char *str = (char*) malloc(2*col*(row+1) + 1);
    int count = 0;
    for(int i=0; i<img->height(); i+=WINDOW_SIZE) {
        //Bottle& row = bt.addList();
        for(int j=0; j<img->width(); j+=WINDOW_SIZE) {
            // calc avg over a subrect
            unsigned int sum_pix = 0;
            int pix_count = 0;
            for(int x=i; x< std::min(img->height(), i+WINDOW_SIZE); x++) {
                for(int y=j; y<std::min(img->width(), j+WINDOW_SIZE); y++) {
                    sum_pix += img->pixel(y,x).r * 0.21+ img->pixel(y,x).g * 0.72 + img->pixel(y,x).b *0.07;
                    pix_count++;
                }
            }
            sum_pix = sum_pix / pix_count;
           int index = std::max(int(floor(sum_pix/11))-1, 0);
           index = index % 11;
           str[count++] = available[index];
           str[count++] = ' ';
        }
        str[count++] = '\n';
        //printf("count %d\n", count);
        //sprintf(str, "\n");
    }
    str[count++] = '\0';
    //printf("%s\n", str);
    bt.addInt32(0);
    bt.addString(str);
    th.setPortWriter(&bt);
    free(str);
    return th;
}
