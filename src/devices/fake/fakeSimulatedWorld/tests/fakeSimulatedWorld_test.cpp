/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/ISimulatedWorld.h>
#include <yarp/dev/tests/ISimulatedWorldTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeSimulatedWorld", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeSimulatedWorld", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeImu device")
    {
        PolyDriver ddmc;
        yarp::dev::ISimulatedWorld* isim=nullptr;

        ////////"Checking opening fakeImu polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeSimulatedWorld");
            REQUIRE(ddmc.open(p_cfg));
        }

        ddmc.view(isim);
        yarp::dev::tests::exec_ISimulatedWorld_test_1(isim);

        //"Close all polydrivers and check"
        {
            CHECK(ddmc.close());
        }
    }

    Network::setLocalMode(false);
}
