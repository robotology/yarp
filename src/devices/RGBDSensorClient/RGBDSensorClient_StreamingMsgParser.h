/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H
#define YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H

#include <yarp/os/LogStream.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/Stamp.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/DeviceDriver.h>

#include <list>
#include <mutex>


class RgbImageReader_Impl :
        public yarp::os::TypedReaderCallback<yarp::sig::FlexImage>
{
private:
    yarp::sig::FlexImage last_rgb;
    mutable std::mutex mutex;

public:
    RgbImageReader_Impl();
    ~RgbImageReader_Impl() override;

    using yarp::os::TypedReaderCallback<yarp::sig::FlexImage>::onRead;
    void onRead(yarp::sig::FlexImage& datum) override;
    yarp::sig::FlexImage getImage();
};


class FloatImageReader_Impl :
        public yarp::os::TypedReaderCallback<yarp::sig::ImageOf< yarp::sig::PixelFloat>>
{
private:
    yarp::sig::ImageOf< yarp::sig::PixelFloat> last_depth;
    mutable std::mutex mutex;

public:
    FloatImageReader_Impl();
    ~FloatImageReader_Impl() override;

    using yarp::os::TypedReaderCallback<yarp::sig::ImageOf< yarp::sig::PixelFloat>>::onRead;
    void onRead(yarp::sig::ImageOf< yarp::sig::PixelFloat> & datum)  override;
    yarp::sig::ImageOf<yarp::sig::PixelFloat> getImage();
};


class RGBDSensor_StreamingMsgParser
{
private:
    RgbImageReader_Impl   read_rgb;
    FloatImageReader_Impl read_depth;

    yarp::os::BufferedPort<yarp::sig::FlexImage> *port_rgb {nullptr};
    yarp::os::BufferedPort<yarp::sig::ImageOf< yarp::sig::PixelFloat>> *port_depth {nullptr};

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

    void attach(yarp::os::BufferedPort<yarp::sig::FlexImage> *_port_rgb,
                yarp::os::BufferedPort<yarp::sig::ImageOf< yarp::sig::PixelFloat>> *_port_depth);
};

#endif  // YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H
