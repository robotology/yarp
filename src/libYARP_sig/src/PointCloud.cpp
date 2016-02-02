/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/sig/PointCloud.hpp>
#include <yarp/os/LogStream.h>

using namespace yarp::sig;

yarp::sig::PointCloud::PointCloud()
{
    xyzColor.resize(header.width * header.height);
    rgba.resize(header.width* header.height);
}

PointCloud::~PointCloud()
{

}

bool yarp::sig::PointCloud::read(yarp::os::ConnectionReader& reader)
{
    yTrace();
    return true;
}

bool yarp::sig::PointCloud::write(yarp::os::ConnectionWriter& writer)
{
    yTrace();
    return true;
}

yarp::os::Type yarp::sig::PointCloud::getType()
{
    return yarp::os::Type::byName("yarp/pointCloud");
}

bool yarp::sig::PointCloud::setPointType(int type)
{
    yTrace();
    pointType = type;
    return true;
}

bool yarp::sig::PointCloud::getPointType(int *type)
{
    yTrace();
    *type = pointType;
    return true;
}

