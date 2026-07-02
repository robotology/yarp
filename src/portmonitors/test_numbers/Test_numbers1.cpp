/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Test_numbers1.h"
#include <yarp/os/LogComponent.h>
#include <yarp/os/Value.h>
#include <vector>

using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(TEST_NUMBERS, "Test_numbers")

void split(const std::string& s, char delim, std::vector<std::string>& elements)
{
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        elements.push_back(item);
    }
}
} //anonymous namespace


void Test_numbers1::getParamsFromCommandLine(std::string carrierString, yarp::os::Property& prop)
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

bool Test_numbers1::create(const yarp::os::Property &options)
{
    yCDebug(TEST_NUMBERS, "created!\n");
    yCDebug(TEST_NUMBERS, "I am attached to the %s\n",
            (options.find("sender_side").asBool()) ? "sender side" : "receiver side");

    //parse the user parameters
    yarp::os::Property m_user_params;
    yCDebug(TEST_NUMBERS) << "user params:" << options.toString();
    std::string str = options.find("carrier").asString();

    m_valueoperation = options.find("value").asInt64();

    return true;
}

void Test_numbers1::destroy()
{
}

bool Test_numbers1::setparam(const yarp::os::Property &params)
{
    return false;
}

bool Test_numbers1::getparam(yarp::os::Property &params)
{
    return false;
}

bool Test_numbers1::accept(yarp::os::Things &thing)
{
    yarp::os::Value *s = thing.cast_as<yarp::os::Value>();
    if (s == NULL)
    {
        yCWarning(TEST_NUMBERS, "expected type `yarp::os::Value(int)` but got wrong data type!\n");
        return false;
    }

    return true;
}

yarp::os::Things & Test_numbers1::update(yarp::os::Things &thing)
{
    //get data to process
    yarp::os::Value *s = thing.cast_as<yarp::os::Value>();
    if (s == NULL)
    {
        yCWarning(TEST_NUMBERS, "expected type `yarp::os::Value(int)` but got wrong data type!\n");
        return thing;
    }

    // add markers at the edges of the sound
    int v = s->asInt64() + 1;
    *s = yarp::os::Value(v);

    //send data
    m_th.setPortWriter(s);

    return m_th;
}

void Test_numbers1::trig()
{
}
