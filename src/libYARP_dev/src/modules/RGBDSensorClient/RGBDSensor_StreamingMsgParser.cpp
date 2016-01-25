/*
* Copyright (C) 2016 RobotCub Consortium
* Author: Alberto Cardellino
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <yarp/os/BufferedPort.h>
#include <yarp/os/LogStream.h>
#include "RGBDSensor_StreamingMsgParser.h"


using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

FlexImageReader_Impl::FlexImageReader_Impl() :  id(UNKNOWN), p(NULL)  {}

FlexImageReader_Impl::~FlexImageReader_Impl()
{
    id = UNKNOWN;
    p  = NULL;
}

void FlexImageReader_Impl::configure(IFlexImageReader *_p, RGBD_ImageType i)
{
    p = _p;
    id = i;
};

void FlexImageReader_Impl::onRead(yarp::sig::FlexImage& datum)
{
    if(p != NULL)
        p->updateImage(id, datum);
    else
        yError() << "FlexImageReader not configured!";
};

RGBDSensor_StreamingMsgParser::RGBDSensor_StreamingMsgParser()
{
    policy = latest;
    read_1.configure(this, DEPTH_IMAGE);
    read_2.configure(this, RGB_IMAGE);
};

bool RGBDSensor_StreamingMsgParser::updateImage(RGBD_ImageType id, yarp::sig::FlexImage data)
{
    switch(id)
    {
        case DEPTH_IMAGE:
            last_1 = data;
        break;

        case RGB_IMAGE:
            last_2 = data;
        break;

        default:
            yError() << "Unknown image type received" << id;
        break;
    }
    return true;
};

void RGBDSensor_StreamingMsgParser::attach(yarp::os::BufferedPort<yarp::sig::FlexImage> *port_1, yarp::os::BufferedPort<yarp::sig::FlexImage> *port_2)
{
    port_1->useCallback(read_1);
    port_2->useCallback(read_2);
}

bool RGBDSensor_StreamingMsgParser::synchRead(yarp::sig::FlexImage &data_1, yarp::sig::FlexImage &data_2)
{
    data_1 = last_1;
    data_2 = last_2;
    return true;
}
