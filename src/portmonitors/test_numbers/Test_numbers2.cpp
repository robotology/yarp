/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Test_numbers2.h"
#include <yarp/os/LogComponent.h>
#include <yarp/os/Value.h>
#include <vector>

using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(TEST_NUMBERS, "Test_numbers")

} //anonymous namespace

bool Test_numbers2::create(const yarp::os::Property &options)
{
    yCDebug(TEST_NUMBERS, "created!\n");
    yCDebug(TEST_NUMBERS, "I am attached to the %s\n",
            (options.find("sender_side").asBool()) ? "sender side" : "receiver side");

    //parse the user parameters
    yarp::os::Property m_user_params;
    yCDebug(TEST_NUMBERS) << "user params:" << options.toString();

    m_valueoperation = options.find("value").asInt64();

    return true;
}

void Test_numbers2::destroy()
{
}

bool Test_numbers2::setparam(const yarp::os::Property &params)
{
    return false;
}

bool Test_numbers2::getparam(yarp::os::Property &params)
{
    return false;
}

bool Test_numbers2::accept(yarp::os::Things &thing)
{
    yarp::os::Value *s = thing.cast_as<yarp::os::Value>();
    if (s == NULL)
    {
        yCWarning(TEST_NUMBERS, "expected type `yarp::os::Value(int)` but got wrong data type!\n");
        return false;
    }

    return true;
}

yarp::os::Things & Test_numbers2::update(yarp::os::Things &thing)
{
    //get data to process
    yarp::os::Value *s = thing.cast_as<yarp::os::Value>();
    if (s == NULL)
    {
        yCWarning(TEST_NUMBERS, "expected type `yarp::os::Value(int)` but got wrong data type!\n");
        return thing;
    }

    int v = s->asInt64() * 2;
    *s = yarp::os::Value(v);

    //send data
    m_th.setPortWriter(s);

    return m_th;
}

void Test_numbers2::trig()
{
}
