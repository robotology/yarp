/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Runnable.h>


yarp::os::Runnable::~Runnable()
{
}

void yarp::os::Runnable::run()
{
}

void yarp::os::Runnable::close()
{
}

void yarp::os::Runnable::beforeStart()
{
}

void yarp::os::Runnable::afterStart(bool success)
{
    YARP_UNUSED(success);
}

bool yarp::os::Runnable::threadInit()
{
    return true;
}

void yarp::os::Runnable::threadRelease()
{
}
