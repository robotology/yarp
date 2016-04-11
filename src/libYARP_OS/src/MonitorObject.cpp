/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/MonitorObject.h>


yarp::os::MonitorObject::~MonitorObject()
{
}

bool yarp::os::MonitorObject::create(const yarp::os::Property& options)
{
    return true;
}

void yarp::os::MonitorObject::destroy()
{
}

bool yarp::os::MonitorObject::setparam(const yarp::os::Property& params)
{
    return false;
}

bool yarp::os::MonitorObject::getparam(yarp::os::Property& params)
{
    return false;
}

void yarp::os::MonitorObject::trig()
{
}

bool yarp::os::MonitorObject::accept(yarp::os::Things& thing)
{
    return true;
}

yarp::os::Things& yarp::os::MonitorObject::update(yarp::os::Things& thing)
{
    return thing;
}

yarp::os::Things& yarp::os::MonitorObject::updateReply(yarp::os::Things& thing)
{
    return thing;
}
