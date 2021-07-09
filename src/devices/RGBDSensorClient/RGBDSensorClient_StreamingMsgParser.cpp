/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RGBDSensorClient_StreamingMsgParser.h"

#include <yarp/os/Time.h>

using namespace yarp::dev;

// Callback reader for rgb
void RgbImageBufferedPort::onRead(yarp::sig::FlexImage& datum)
{
    std::lock_guard<std::mutex> lock(mutex);
    local_arrival_time = yarp::os::Time::now();
    std::swap(datum, last_rgb);
    getEnvelope(stamp);
}

std::tuple<bool, yarp::sig::FlexImage, yarp::os::Stamp> RgbImageBufferedPort::getImage() const
{
    std::lock_guard<std::mutex> lock(mutex);
    if (local_arrival_time <= 0.0) {
        // No image received yet
        // FIXME C++17:
        // return {false, yarp::sig::FlexImage(), yarp::os::Stamp()};
        // or perhaps just return {false, {}, {}}; ?
        return std::make_tuple(false, yarp::sig::FlexImage(), yarp::os::Stamp());
    }
    // FIXME C++17:
    // return {true, last_rgb, stamp};
    return std::make_tuple(true, last_rgb, stamp);
}


// callback reader for depthImage
void FloatImageBufferedPort::onRead(yarp::sig::ImageOf< yarp::sig::PixelFloat> & datum)
{
    std::lock_guard<std::mutex> lock(mutex);
    local_arrival_time = yarp::os::Time::now();
    std::swap(datum, last_depth);
    getEnvelope(stamp);
}

std::tuple<bool, yarp::sig::ImageOf<yarp::sig::PixelFloat>, yarp::os::Stamp> FloatImageBufferedPort::getImage() const
{
    std::lock_guard<std::mutex> lock(mutex);
    if (local_arrival_time <= 0.0) {
        // No image received yet
        // FIXME C++17:
        // return {false, yarp::sig::ImageOf<yarp::sig::PixelFloat>(), yarp::os::Stamp()};
        // or perhaps just return {false, {}, {}}; ?
        return std::make_tuple(false, yarp::sig::ImageOf<yarp::sig::PixelFloat>(), yarp::os::Stamp());

    }
    // FIXME C++17:
    // return {true, last_depth, stamp};
    return std::make_tuple(true, last_depth, stamp);
}


// Streaming handler
bool RGBDSensor_StreamingMsgParser::readRgb(yarp::sig::FlexImage &data, yarp::os::Stamp *timeStamp)
{
    auto result = port_rgb->getImage();

    if (!std::get<0>(result)) {
        return false;
    }

    data = std::get<1>(result);
    if (timeStamp) {
        *timeStamp = std::get<2>(result);
    }

    return true;
}

bool RGBDSensor_StreamingMsgParser::readDepth(yarp::sig::ImageOf< yarp::sig::PixelFloat > &data, yarp::os::Stamp *timeStamp)
{
    auto result = port_depth->getImage();

    if (!std::get<0>(result)) {
        return false;
    }

    data = std::get<1>(result);
    if(timeStamp) {
        *timeStamp = std::get<2>(result);
    }

    return true;
}

bool RGBDSensor_StreamingMsgParser::read(yarp::sig::FlexImage &rgbImage, yarp::sig::ImageOf< yarp::sig::PixelFloat > &depthImage, yarp::os::Stamp *rgbStamp, yarp::os::Stamp *depthStamp)
{
    auto resultRgb = port_rgb->getImage();
    auto resultDepth = port_depth->getImage();

    bool retRgb = std::get<0>(resultRgb);
    bool retDepth = std::get<0>(resultDepth);

    if (!retRgb || !retDepth) {
        return false;
    }

    rgbImage = std::get<1>(resultRgb);
    depthImage = std::get<1>(resultDepth);
    if(rgbStamp) {
        port_rgb->getEnvelope(*rgbStamp);
    }

    if(depthStamp) {
        port_depth->getEnvelope(*depthStamp);
    }
    return true;
}

void RGBDSensor_StreamingMsgParser::attach(RgbImageBufferedPort* _port_rgb,
                                           FloatImageBufferedPort* _port_depth)
{
    port_rgb = _port_rgb;
    port_depth = _port_depth;
    port_rgb->useCallback();
    port_depth->useCallback();
}
