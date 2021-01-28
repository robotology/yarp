/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "Nop.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>


namespace {
YARP_LOG_COMPONENT(NOP, "yarp.device.test_nop")
}

Nop::Nop() :
        PeriodicThread(1.0)
{
}

Nop::~Nop()
{
}

bool Nop::open(yarp::os::Searchable& config)
{
    if (config.check("period")) {
        period = config.find("period").asFloat64();
    }
    setPeriod(period);

    yCInfo(NOP) << "Starting thread";
    start();
    yCInfo(NOP) << "Thread started";

    return true;
}

bool Nop::close()
{
    return true;
}

void Nop::run()
{
    yCInfo(NOP) << "Doing nothing";
}
