/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Simulated_network_delay.h"
#include <yarp/os/Value.h>
#include <yarp/os/Time.h>
#include <vector>

#include <iostream>

using namespace yarp::os;

namespace {

} //anonymous namespace

bool Simulated_network_delay::create(const yarp::os::Property &options)
{
    //parse the user parameters
    std::string debugs = options.toString();
    YARP_UNUSED (debugs);

    if (options.check("delay_ms"))
    {
        m_delay = options.find("delay_ms").asFloat32();
        m_delay /= 1000.0;
    }

    return true;
}

void Simulated_network_delay::destroy()
{
}

bool Simulated_network_delay::setparam(const yarp::os::Property &params)
{
    return false;
}

bool Simulated_network_delay::getparam(yarp::os::Property &params)
{
    return false;
}

bool Simulated_network_delay::accept(yarp::os::Things &thing)
{
    return true;
}

yarp::os::Things & Simulated_network_delay::update(yarp::os::Things &thing)
{
    yarp::os::Time::delay(m_delay);
    return thing;
}

void Simulated_network_delay::trig()
{
}
