/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::TestDeviceWGP", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("testDeviceWGP", "device");

    Network::setLocalMode(true);

    SECTION("Checking TestDeviceWGP device")
    {
        PolyDriver dd;

        ////////"Checking opening polydriver with no attached device"
        {
            Property p_cfg;
            p_cfg.put("device",    "testDeviceWGP");
            p_cfg.put("file_name", "mandatory_name");
            p_cfg.put("period",     1.0);

            Property& pp_cfg = p_cfg.addGroup("group3");
            Property& ppp_cfg = pp_cfg.addGroup("subgroup1");
            ppp_cfg.put("param_1", "test_string");

            REQUIRE(dd.open(p_cfg));
        }

        yarp::os::Time::delay(1.0);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
