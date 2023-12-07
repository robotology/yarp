/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/tests/IBatteryTest.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeBattery", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeBattery", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeBattery device")
    {
        PolyDriver dd;
        yarp::dev::IBattery* ibatt=nullptr;

        ////////"Checking opening device polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeBattery");
            REQUIRE(dd.open(p_cfg));
        }

        dd.view(ibatt);
        yarp::dev::tests::exec_iBattery_test_1(ibatt);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
