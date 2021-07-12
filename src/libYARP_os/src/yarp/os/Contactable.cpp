/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
