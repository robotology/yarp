/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/tests/ICartesianControlTest.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeCartesianControl", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeCartesianControl", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeCartesianControl device")
    {
        PolyDriver dd;
        yarp::dev::ICartesianControl* icart=nullptr;

        ////////"Checking opening device polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeCartesianControl");
            REQUIRE(dd.open(p_cfg));
        }

        dd.view(icart);
        yarp::dev::tests::exec_iCartesian_test_1(icart);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
