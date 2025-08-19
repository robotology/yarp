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

void split(const std::string& s, char delim, std::vector<std::string>& elements)
{
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        elements.push_back(item);
    }
}
} //anonymous namespace


void Simulated_network_delay::getParamsFromCommandLine(std::string carrierString, yarp::os::Property& prop)
{
    // Split command line string using '+' delimiter
    std::vector<std::string> parameters;
    split(carrierString, '+', parameters);

    // Iterate over result strings
    for (std::string param : parameters)
    {
        // If there is no '.', then the param is bad formatted, skip it.
        auto pointPosition = param.find('.');
        if (pointPosition == std::string::npos)
        {
            continue;
        }

        // Otherwise, separate key and value
        std::string paramKey = param.substr(0, pointPosition);
        yarp::os::Value paramValue;
        std::string s = param.substr(pointPosition + 1, param.length());
        paramValue.fromString(s.c_str());

        //and append to the returned property
        prop.put(paramKey, paramValue);
    }
    return;
}

bool Simulated_network_delay::create(const yarp::os::Property &options)
{
    //parse the user parameters
    yarp::os::Property m_user_params;
    std::string str = options.find("carrier").asString();
    getParamsFromCommandLine(str, m_user_params);

    if (m_user_params.check("delay_ms"))
    {
        m_delay = m_user_params.find("delay_ms").asFloat32();
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
