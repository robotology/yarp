/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Contactable.h>

#include <yarp/os/AbstractContactable.h>


yarp::os::Contactable::~Contactable() = default;

std::string yarp::os::Contactable::getName() const
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
