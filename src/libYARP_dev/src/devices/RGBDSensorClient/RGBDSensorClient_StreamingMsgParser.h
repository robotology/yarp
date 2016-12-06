/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#ifndef YARP_DEV_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H
#define YARP_DEV_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H

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
    void onRead(yarp::sig::FlexImage& datum);
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
    void onRead(yarp::sig::ImageOf< yarp::sig::PixelFloat> & datum);
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

#endif  // YARP_DEV_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H
