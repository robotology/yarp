/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "SegFault.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

SegFault::SegFault()
{
}

SegFault::~SegFault()
{
}

bool SegFault::open(yarp::os::Searchable& config)
{
    if (config.check("delay")) {
        delay = config.find("delay").asFloat64();
    }

    yInfo() << "[segfault] Starting thread";
    start();
    yInfo() << "[segfault] Thread started";

    return true;
}

bool SegFault::close()
{
    return true;
}

void SegFault::run()
{
    yInfo() << "[segfault] Waiting for" << delay << "seconds before crashing";
    yarp::os::Time::delay(delay);
    yWarning() << "[segfault] Crashing";
    *(int*) 0 = 0;
}
