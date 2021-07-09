/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "AsciiImage.h"

#include <yarp/os/LogComponent.h>

#include <yarp/sig/Image.h>

#include <algorithm>
#include <cmath>
#include <cstdio>


namespace {
YARP_LOG_COMPONENT(ASCIIIMAGE,
                   "yarp.carrier.portmonitor.asciiimage",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)

constexpr size_t WINDOW_SIZE = 6;

constexpr double r_perc = 0.21;
constexpr double g_perc = 0.72;
constexpr double b_perc = 0.07;

static char available[] = {' ', '`', '.', '~', '+', 'I', 'X', 'O', '8', '%', 'W'};
}


using namespace yarp::os;
using namespace yarp::sig;



bool AsciiImageMonitorObject::create(const yarp::os::Property& options)
{
    YARP_UNUSED(options);
    yCInfo(ASCIIIMAGE, "created!");
    return true;
}

void AsciiImageMonitorObject::destroy()
{
    yCInfo(ASCIIIMAGE, "destroyed!");
}

bool AsciiImageMonitorObject::setparam(const yarp::os::Property& params)
{
    YARP_UNUSED(params);
    return false;
}

bool AsciiImageMonitorObject::getparam(yarp::os::Property& params)
{
    YARP_UNUSED(params);
    return false;
}

bool AsciiImageMonitorObject::accept(yarp::os::Things& thing)
{
    ImageOf<PixelRgb>* img = thing.cast_as<ImageOf<PixelRgb>>();
    if (img == nullptr) {
        yCError(ASCIIIMAGE, "AsciiImageMonitorObject: expected type ImageOf<PixelRgb> but got wrong data type!");
        return false;
    }

    return true;
}

yarp::os::Things& AsciiImageMonitorObject::update(yarp::os::Things& thing)
{
    ImageOf<PixelRgb>* img = thing.cast_as<ImageOf<PixelRgb>>();
    bt.clear();
    auto col = img->width() / WINDOW_SIZE + 1;
    auto row = img->height() / WINDOW_SIZE + 1;

    char* str = static_cast<char*>(malloc(2 * col * (row + 1) + 1));
    int count = 0;
    for (int i = 0; i < img->height(); i += WINDOW_SIZE) {
        //Bottle& row = bt.addList();
        for (int j = 0; j < img->width(); j += WINDOW_SIZE) {
            // calc avg over a subrect
            double sum_pix = 0;
            int pix_count = 0;
            for (int x = i; x < std::min(img->height(), i + WINDOW_SIZE); x++) {
                for (int y = j; y < std::min(img->width(), j + WINDOW_SIZE); y++) {
                    sum_pix += img->pixel(y, x).r * r_perc + img->pixel(y, x).g * g_perc + img->pixel(y, x).b * b_perc;
                    pix_count++;
                }
            }
            sum_pix = sum_pix / pix_count;
            int index = std::max(static_cast<int>(floor(sum_pix / sizeof(available))) - 1, 0);
            index = index % sizeof(available);
            str[count++] = available[index];
            str[count++] = ' ';
        }
        str[count++] = '\n';
        //yCTrace(ASCIIIMAGE, "count %d\n", count);
    }
    str[count++] = '\0';
    //yCTrace(ASCIIIMAGE, "%s\n", str);
    bt.addInt32(0);
    bt.addString(str);
    th.setPortWriter(&bt);
    free(str);
    return th;
}
