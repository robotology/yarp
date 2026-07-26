/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "imageRotation.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/sig/Image.h>
#include <yarp/sig/ImageUtils.h>

#include <algorithm>
#include <cmath>

using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(IMAGEROTATION,
                   "yarp.carrier.portmonitor.image_rotation",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)

} //anonymous namespace

bool ImageRotation::create(const yarp::os::Property& options)
{
    if (options.check("options_rotate"))
    {
        const std::string val = options.find("options_rotate").asString();
        if      (val == "rotate_cw")  { m_rotate_option = yarp::sig::utils::RotateOption::rotate_cw; }
        else if (val == "rotate_ccw") { m_rotate_option = yarp::sig::utils::RotateOption::rotate_ccw; }
        else if (val == "rotate_180") { m_rotate_option = yarp::sig::utils::RotateOption::rotate_180; }
        else if (val == "rotate_none"){ m_rotate_option = yarp::sig::utils::RotateOption::rotate_none; }
        else
        {
            yCDebug(IMAGEROTATION) << "Invalid value of `options_rotate` parameter:" << val;
            return false;
        }
    }

    if (options.check("options_flip"))
    {
        const std::string val = options.find("options_flip").asString();
        if      (val == "flip_x")   { m_flip_option = yarp::sig::utils::FlipOption::flip_x; }
        else if (val == "flip_y")   { m_flip_option = yarp::sig::utils::FlipOption::flip_y; }
        else if (val == "flip_xy")  { m_flip_option = yarp::sig::utils::FlipOption::flip_xy; }
        else if (val == "flip_none"){ m_flip_option = yarp::sig::utils::FlipOption::flip_none; }
        else
        {
            yCDebug(IMAGEROTATION) << "Invalid value of `options_flip` parameter:" << val;
            return false;
        }
    }

    return true;
}

void ImageRotation::destroy()
{
}

bool ImageRotation::setparam(const yarp::os::Property& params)
{
    return false;
}

bool ImageRotation::getparam(yarp::os::Property& params)
{
    return false;
}

bool ImageRotation::accept(yarp::os::Things& thing)
{
    auto* img = thing.cast_as<yarp::sig::Image>();
    if (img == nullptr) {
        yCError(IMAGEROTATION, "Expected type Image, but got wrong data type!");
        return false;
    }
    if (img->getPixelCode() != VOCAB_PIXEL_RGB && img->getPixelCode() != VOCAB_PIXEL_MONO_FLOAT) {
        yCError(IMAGEROTATION, "Received image with invalid/unsupported pixelCode!");
        return false;
    }
    return true;
}

yarp::os::Things& ImageRotation::update(yarp::os::Things& thing)
{
    yarp::sig::Image* yarpimg = thing.cast_as<yarp::sig::Image>();
    if (yarpimg->getPixelCode() == VOCAB_PIXEL_RGB)
    {
       yarp::sig::utils::rotate(m_outImgRgb, *yarpimg, m_rotate_option, m_flip_option);
        m_th.setPortWriter(&m_outImgRgb);
    }
    else if (yarpimg->getPixelCode() == VOCAB_PIXEL_MONO_FLOAT)
    {
       yarp::sig::utils::rotate(m_outImgFloat, *yarpimg, m_rotate_option, m_flip_option);
        m_th.setPortWriter(&m_outImgFloat);
    }
    else {
        yCError(IMAGEROTATION, "Invalid Image type!");
    }
    return m_th;
}
