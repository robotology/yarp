/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ThrottleDown.h"

#include <algorithm>
#include <cmath>

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(PM_THRD,
                   "yarp.carrier.portmonitor.ThrottleDown",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)

void split(const std::string& s, char delim, std::vector<std::string>& elements)
{
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim))
    {
        elements.push_back(item);
    }
}

}//anonymous namespace


bool ThrottleDown::create(const yarp::os::Property& options)
{
    // parse the user parameters
    yarp::os::Property m_user_params;
    std::string str = options.find("carrier").asString();
    yCDebug(PM_THRD) << "parsed string:" << str << " "<< options.toString();
    getParamsFromCommandLine(str, m_user_params);
    yCDebug(PM_THRD) << "parsed params:" << m_user_params.toString();

    // get the value of the parameters
    /* if (m_user_params.check("period")) {
        //Float64 have issues related to the '.' character.
        //Even replacing the separator character in getParamsFromCommandLine() from ` ` to `=`
        //0.02 becomes 0.2, probabibly located in protocol .cpp
        m_period = m_user_params.find("period").asFloat64();
        yCDebug(PM_THRD) << "period set:" << m_period;
    }*/
    if (m_user_params.check("period_ms")) {
        m_period = m_user_params.find("period_ms").asInt32();
        m_period /= 1000.0;
        yCDebug(PM_THRD) << "period set:" << m_period;
    }

    m_last_time= yarp::os::Time::now();

    return true;
}

void ThrottleDown::destroy()
{
}

void ThrottleDown::getParamsFromCommandLine(std::string carrierString, yarp::os::Property& prop)
{
    // Split command line string using '+' delimiter
    std::vector<std::string> parameters;
    split(carrierString, '+', parameters);

    // Iterate over result strings
    for (std::string param : parameters) {
        // If there is no '.', then the param is bad formatted, skip it.
        auto pointPosition = param.find('.');
        if (pointPosition == std::string::npos) {
            continue;
        }

        // Otherwise, separate key and value
        std::string paramKey = param.substr(0, pointPosition);
        yarp::os::Value paramValue;
        std::string s = param.substr(pointPosition + 1, param.length());
        paramValue.fromString(s.c_str());

        // and append to the returned property
        prop.put(paramKey, paramValue);
    }
    return;
}


bool ThrottleDown::setparam(const yarp::os::Property& params)
{
    return false;
}

bool ThrottleDown::getparam(yarp::os::Property& params)
{
    return false;
}

bool ThrottleDown::accept(yarp::os::Things& thing)
{
    double cur_time = yarp::os::Time::now();
    if (cur_time - m_last_time > m_period)
    {
        m_last_time = cur_time;
        return true;
    }
    return false;
}

yarp::os::Things& ThrottleDown::update(yarp::os::Things& thing)
{
    return thing;
}
