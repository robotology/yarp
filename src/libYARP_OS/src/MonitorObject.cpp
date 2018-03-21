/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/MonitorObject.h>
#include <yarp/os/Property.h>
#include <yarp/os/Things.h>

yarp::os::MonitorObject::~MonitorObject()
{
}

bool yarp::os::MonitorObject::create(const yarp::os::Property& options)
{
    YARP_UNUSED(options);
    return true;
}

void yarp::os::MonitorObject::destroy()
{
}

bool yarp::os::MonitorObject::setparam(const yarp::os::Property& params)
{
    YARP_UNUSED(params);
    return false;
}

bool yarp::os::MonitorObject::getparam(yarp::os::Property& params)
{
    YARP_UNUSED(params);
    return false;
}

void yarp::os::MonitorObject::trig()
{
}

bool yarp::os::MonitorObject::accept(yarp::os::Things& thing)
{
    YARP_UNUSED(thing);
    return true;
}

yarp::os::Things& yarp::os::MonitorObject::update(yarp::os::Things& thing)
{
    YARP_UNUSED(thing);
    return thing;
}

yarp::os::Things& yarp::os::MonitorObject::updateReply(yarp::os::Things& thing)
{
    YARP_UNUSED(thing);
    return thing;
}
