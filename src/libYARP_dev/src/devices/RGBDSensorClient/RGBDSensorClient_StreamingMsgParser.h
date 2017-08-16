/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Author: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H
#define YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H

#include <list>
#include <yarp/os/Stamp.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/LogStream.h>

namespace yarp {
    namespace dev {
        class depthCameraDriver;
        namespace impl {
            class  RgbImageReader_Impl;
            class  FloatImageReader_Impl;
            class  RGBDSensor_StreamingMsgParser;
        }
    }
}


class YARP_dev_API yarp::dev::impl::RgbImageReader_Impl:  public yarp::os::TypedReaderCallback<yarp::sig::FlexImage>
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


class YARP_dev_API yarp::dev::impl::FloatImageReader_Impl:  public yarp::os::TypedReaderCallback<yarp::sig::ImageOf< yarp::sig::PixelFloat> >
{
private:
    yarp::sig::ImageOf< yarp::sig::PixelFloat>  last_depth;

public:
    FloatImageReader_Impl();
    ~FloatImageReader_Impl();

    using yarp::os::TypedReaderCallback<yarp::sig::ImageOf< yarp::sig::PixelFloat> >::onRead;
    void onRead(yarp::sig::ImageOf< yarp::sig::PixelFloat> & datum)  override;
    yarp::sig::ImageOf<yarp::sig::PixelFloat> getImage();
};


class yarp::dev::impl::RGBDSensor_StreamingMsgParser
{
private:
    RgbImageReader_Impl   read_rgb;
    FloatImageReader_Impl read_depth;

    yarp::os::BufferedPort<yarp::sig::FlexImage> *port_rgb;
    yarp::os::BufferedPort<yarp::sig::ImageOf< yarp::sig::PixelFloat> > *port_depth;

public:
    RGBDSensor_StreamingMsgParser();

    bool readRgb(yarp::sig::FlexImage &data, yarp::os::Stamp *timeStamp = NULL);

    bool readDepth(yarp::sig::ImageOf< yarp::sig::PixelFloat > &data, yarp::os::Stamp *timeStamp = NULL);

    bool read(yarp::sig::FlexImage &rgbImage, yarp::sig::ImageOf< yarp::sig::PixelFloat > &depthImage, yarp::os::Stamp *rgbStamp = NULL, yarp::os::Stamp *depthStamp = NULL);

    void attach(yarp::os::BufferedPort<yarp::sig::FlexImage> *_port_rgb,
                yarp::os::BufferedPort<yarp::sig::ImageOf< yarp::sig::PixelFloat> > *_port_depth);
};

#endif  // YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H
