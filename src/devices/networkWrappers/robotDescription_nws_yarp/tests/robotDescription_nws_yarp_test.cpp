/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRobotDescription.h>
#include <yarp/dev/IWrapper.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>

#include <algorithm>
#include <vector>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;

TEST_CASE("dev::robotDescription_nws_yarp_Test", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("robotDescription_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("robotDescriptionStorage", "device");

    Network::setLocalMode(true);

    SECTION("Test the RobotDescription_nws_yarp device")
    {
        PolyDriver ddserver;
        Property pserver_cfg;
        pserver_cfg.put("device", "robotDescription_nws_yarp");
        pserver_cfg.put("local", "/robotDescription_nws_yarp/rpc");
        REQUIRE(ddserver.open(pserver_cfg)); // robotDescription_nws_yarp open reported successful

        yarp::os::Time::delay(1.0);

        // Close devices
        CHECK(ddserver.close()); // robotDescription_nws_yarp successfully closed
    }

    SECTION("Test the RobotDescription_nws_yarp device with attach")
    {
        PolyDriver dd_stor;
        Property p_cfg_stor;
        p_cfg_stor.put("device", "robotDescriptionStorage");
        REQUIRE(dd_stor.open(p_cfg_stor));

        PolyDriver ddnws;
        Property pserver_cfg;
        pserver_cfg.put("device", "robotDescription_nws_yarp");
        pserver_cfg.put("local", "/robotDescription_nws_yarp/rpc");
        REQUIRE(ddnws.open(pserver_cfg)); // robotDescription_nws_yarp open reported successful

        yarp::dev::IWrapper* ww_nws = nullptr; ddnws.view(ww_nws);
        REQUIRE(ww_nws);
        bool result_att = ww_nws->attach(&dd_stor);
        REQUIRE(result_att);

        yarp::os::Time::delay(1.0);

        // Close devices
        CHECK(ddnws.close()); // robotDescription_nws_yarp successfully closed
    }

    Network::setLocalMode(false);
}
