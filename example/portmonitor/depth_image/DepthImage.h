// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2016 iCub Facility
 * Authors: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_CARRIER_DEPTHIMAGECONVERTER_H
#define YARP_CARRIER_DEPTHIMAGECONVERTER_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/os/MonitorObject.h>


class DepthImageConverter : public yarp::os::MonitorObject
{
public:
    bool create(const yarp::os::Property& options);
    void destroy(void);

    bool setparam(const yarp::os::Property& params);
    bool getparam(yarp::os::Property& params);

    bool accept(yarp::os::Things& thing);
    yarp::os::Things& update(yarp::os::Things& thing);

private:

    double min, max;
    yarp::os::Bottle bt;
    yarp::os::Things th;
    float **inMatrix;
    unsigned char **outMatrix;
    yarp::sig::FlexImage outImg;
};

#endif  // YARP_CARRIER_DEPTHIMAGECONVERTER_H
