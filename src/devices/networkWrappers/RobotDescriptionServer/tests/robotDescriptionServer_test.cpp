/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRobotDescription.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>

#include <algorithm>
#include <vector>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;

TEST_CASE("dev::robotDescriptionServerTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("robotDescriptionServer", "device");

    Network::setLocalMode(true);

    SECTION("Test the RobotDescriptionServer device")
    {
        PolyDriver ddserver;
        Property pserver_cfg;
        pserver_cfg.put("device", "robotDescriptionServer");
        pserver_cfg.put("local", "/robotDescriptionServerPort");
        REQUIRE(ddserver.open(pserver_cfg)); // robotDescriptionServer open reported successful

        yarp::os::Time::delay(1.0);

        // Close devices
        CHECK(ddserver.close()); // robotDescriptionServer successfully closed
    }

    Network::setLocalMode(false);
}
