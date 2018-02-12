/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Contactable.h>
#include <yarp/os/AbstractContactable.h>


yarp::os::Contactable::~Contactable()
{
}

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.72
bool yarp::os::Contactable::open()
{
    return open("...");
}
#endif // YARP_NO_DEPRECATED

yarp::os::ConstString yarp::os::Contactable::getName() const
{
    return where().getName();
}

void yarp::os::Contactable::setReadOnly()
{
    setInputMode(true);
    setOutputMode(false);
    setRpcMode(false);
}

void yarp::os::Contactable::setWriteOnly()
{
    setInputMode(false);
    setOutputMode(true);
    setRpcMode(false);
}

void yarp::os::Contactable::setRpcServer()
{
    setInputMode(true);
    setOutputMode(false);
    setRpcMode(true);
}

void yarp::os::Contactable::setRpcClient()
{
    setInputMode(false);
    setOutputMode(true);
    setRpcMode(true);
}
