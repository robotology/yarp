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
#include <yarp/sig/Image.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/LogStream.h>

typedef enum
{
    latest,
    sequenceNumber,
    timed
} SynchPolicy;

typedef enum
{
    DEPTH_IMAGE,
    RGB_IMAGE,
    UNKNOWN
} RGBD_ImageType;

class YARP_dev_API IFlexImageReader
{
public:
    virtual bool updateImage(RGBD_ImageType, yarp::sig::FlexImage) = 0;
    virtual ~IFlexImageReader() {};
};

class YARP_dev_API FlexImageReader_Impl:  public yarp::os::TypedReaderCallback<yarp::sig::FlexImage>
{
    RGBD_ImageType id;
    IFlexImageReader *p;

public:
    FlexImageReader_Impl();
    ~FlexImageReader_Impl();
    void configure(IFlexImageReader *_p, RGBD_ImageType i);
    void onRead(yarp::sig::FlexImage& datum);
    using yarp::os::TypedReaderCallback<yarp::sig::FlexImage>::onRead;
};

class RGBDSensor_StreamingMsgParser:    public IFlexImageReader
{
private:
    FlexImageReader_Impl read_1;
    FlexImageReader_Impl read_2;

    yarp::sig::FlexImage last_1;
    yarp::sig::FlexImage last_2;

    SynchPolicy policy;

public:
    RGBDSensor_StreamingMsgParser();
    bool updateImage(RGBD_ImageType id, yarp::sig::FlexImage data);
    bool synchRead(yarp::sig::FlexImage &data_1, yarp::sig::FlexImage &data_2);
    void attach(yarp::os::BufferedPort<yarp::sig::FlexImage> *port_1, yarp::os::BufferedPort<yarp::sig::FlexImage> *port_2);
};

#endif  // YARP_DEV_RGBDSENSORCLIENT_STREAMINGMSGPARSER_H
