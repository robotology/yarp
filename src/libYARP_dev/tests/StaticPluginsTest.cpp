/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Os.h>
#include <yarp/os/Network.h>
#include <yarp/dev/all.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::StaticPluginsTest", "[yarp::dev]")
{
    Network::setLocalMode(true);

    SECTION("Test yarp::dev::Drivers::factory().find()")
    {
        std::string ss = yarp::dev::Drivers::factory().toString();
        size_t pos = ss.find("fakeDeviceUnwrapped");
        CHECK(pos != std::string::npos);
        auto r = yarp::dev::Drivers::factory().find("fakeDeviceUnwrapped");
        CHECK(r);
    }

    Network::setLocalMode(false);
}
