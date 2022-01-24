/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "DepthImageVec.h"

#include <algorithm>
#include <cmath>

#include <yarp/os/LogComponent.h>
#include <yarp/sig/Image.h>

using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(DEPTHIMAGE,
                   "yarp.carrier.portmonitor.depthimage_to_vector",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}

bool DepthImageConverterVec::create(const yarp::os::Property& options)
{
    return true;
}

void DepthImageConverterVec::destroy()
{
}

bool DepthImageConverterVec::setparam(const yarp::os::Property& params)
{
    return false;
}

bool DepthImageConverterVec::getparam(yarp::os::Property& params)
{
    return false;
}

bool DepthImageConverterVec::accept(yarp::os::Things& thing)
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

yarp::os::Things& DepthImageConverterVec::update(yarp::os::Things& thing)
{
    auto* img = thing.cast_as<Image>();

    outVec.resize(img->width()* img->height());

    auto* inPixels = reinterpret_cast<float *> (img->getRawImage());
    std::copy(inPixels, inPixels + outVec.size(), outVec.begin());

    th.setPortWriter(&outVec);
    return th;
}
