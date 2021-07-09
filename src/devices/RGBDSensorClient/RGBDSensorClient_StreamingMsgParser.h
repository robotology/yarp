/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H
#define YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H

#include <yarp/os/LogStream.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/Stamp.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/DeviceDriver.h>

#include <list>
#include <tuple>
#include <mutex>


class RgbImageBufferedPort :
        public yarp::os::BufferedPort<yarp::sig::FlexImage>
{
private:
    double local_arrival_time {0.0};
    yarp::os::Stamp stamp;
    yarp::sig::FlexImage last_rgb;
    mutable std::mutex mutex;

public:
    RgbImageBufferedPort() = default;
    ~RgbImageBufferedPort() override = default;

    using yarp::os::TypedReaderCallback<yarp::sig::FlexImage>::onRead;
    void onRead(yarp::sig::FlexImage& datum) override;
    std::tuple<bool, yarp::sig::FlexImage, yarp::os::Stamp> getImage() const;
};


class FloatImageBufferedPort :
        public yarp::os::BufferedPort<yarp::sig::ImageOf< yarp::sig::PixelFloat>>
{
private:
    double local_arrival_time {0.0};
    yarp::os::Stamp stamp;
    yarp::sig::ImageOf< yarp::sig::PixelFloat> last_depth;
    mutable std::mutex mutex;

public:
    FloatImageBufferedPort() = default;
    ~FloatImageBufferedPort() override = default;

    using yarp::os::TypedReaderCallback<yarp::sig::ImageOf< yarp::sig::PixelFloat>>::onRead;
    void onRead(yarp::sig::ImageOf< yarp::sig::PixelFloat> & datum) override;
    std::tuple<bool, yarp::sig::ImageOf<yarp::sig::PixelFloat>, yarp::os::Stamp> getImage() const;
};


class RGBDSensor_StreamingMsgParser
{
private:
    RgbImageBufferedPort   *port_rgb {nullptr};
    FloatImageBufferedPort *port_depth {nullptr};

public:
    RGBDSensor_StreamingMsgParser() = default;

    bool readRgb(yarp::sig::FlexImage &data,
                 yarp::os::Stamp *timeStamp = nullptr);

    bool readDepth(yarp::sig::ImageOf< yarp::sig::PixelFloat > &data,
                   yarp::os::Stamp *timeStamp = nullptr);

    bool read(yarp::sig::FlexImage &rgbImage,
              yarp::sig::ImageOf< yarp::sig::PixelFloat > &depthImage,
              yarp::os::Stamp *rgbStamp = nullptr,
              yarp::os::Stamp *depthStamp = nullptr);

    void attach(RgbImageBufferedPort* _port_rgb,
                FloatImageBufferedPort* _port_depth);
};

#endif  // YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H
