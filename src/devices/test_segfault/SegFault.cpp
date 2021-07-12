/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SegFault.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

namespace {
YARP_LOG_COMPONENT(SEGFAULT, "yarp.device.test_segfault")
}

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

    yCInfo(SEGFAULT) << "Starting thread";
    start();
    yCInfo(SEGFAULT) << "Thread started";

    return true;
}

bool SegFault::close()
{
    return true;
}

void SegFault::run()
{
    yCInfo(SEGFAULT) << "Waiting for" << delay << "seconds before crashing";
    yarp::os::Time::delay(delay);
    yCWarning(SEGFAULT) << "Crashing";
    *(int*) 0 = 0;
}
