/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H
#define YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H

#include <list>
#include <yarp/os/Stamp.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/LogStream.h>


class RgbImageReader_Impl :
        public yarp::os::TypedReaderCallback<yarp::sig::FlexImage>
{
private:
    yarp::sig::FlexImage  last_rgb;

public:
    RgbImageReader_Impl();
    ~RgbImageReader_Impl();

    using yarp::os::TypedReaderCallback<yarp::sig::FlexImage>::onRead;
    void onRead(yarp::sig::FlexImage& datum) override;
    yarp::sig::FlexImage getImage();
};


class FloatImageReader_Impl :
        public yarp::os::TypedReaderCallback<yarp::sig::ImageOf< yarp::sig::PixelFloat>>
{
private:
    yarp::sig::ImageOf< yarp::sig::PixelFloat>  last_depth;

public:
    FloatImageReader_Impl();
    ~FloatImageReader_Impl();

    using yarp::os::TypedReaderCallback<yarp::sig::ImageOf< yarp::sig::PixelFloat>>::onRead;
    void onRead(yarp::sig::ImageOf< yarp::sig::PixelFloat> & datum)  override;
    yarp::sig::ImageOf<yarp::sig::PixelFloat> getImage();
};


class RGBDSensor_StreamingMsgParser
{
private:
    RgbImageReader_Impl   read_rgb;
    FloatImageReader_Impl read_depth;

    yarp::os::BufferedPort<yarp::sig::FlexImage> *port_rgb;
    yarp::os::BufferedPort<yarp::sig::ImageOf< yarp::sig::PixelFloat>> *port_depth;

public:
    RGBDSensor_StreamingMsgParser();

    bool readRgb(yarp::sig::FlexImage &data, yarp::os::Stamp *timeStamp = NULL);

    bool readDepth(yarp::sig::ImageOf< yarp::sig::PixelFloat > &data, yarp::os::Stamp *timeStamp = NULL);

    bool read(yarp::sig::FlexImage &rgbImage, yarp::sig::ImageOf< yarp::sig::PixelFloat > &depthImage, yarp::os::Stamp *rgbStamp = NULL, yarp::os::Stamp *depthStamp = NULL);

    void attach(yarp::os::BufferedPort<yarp::sig::FlexImage> *_port_rgb,
                yarp::os::BufferedPort<yarp::sig::ImageOf< yarp::sig::PixelFloat>> *_port_depth);
};

#endif  // YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H
