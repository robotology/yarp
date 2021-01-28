/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#include <yarp/os/Runnable.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE

YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING

yarp::os::Runnable::~Runnable() = default;

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

YARP_WARNING_POP
