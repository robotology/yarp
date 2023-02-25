/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRangefinder2D.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IRangefinder2DTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::laserFromDepth", "[yarp::dev]")
{
    #if defined(YARP_DISABLE_FAILING_TESTS)
        YARP_SKIP_TEST("Skipping failing tests")
    #endif

    YARP_REQUIRE_PLUGIN("laserFromDepth", "device");

    Network::setLocalMode(true);

    SECTION("Checking laserFromDepth device")
    {
        PolyDriver laserdev;
        IRangefinder2D* irng = nullptr;

        ////////"Checking opening polydriver"
        {
            Property las_cfg;
            las_cfg.put("device", "laserFromDepth");
            REQUIRE(laserdev.open(las_cfg));
            REQUIRE(laserdev.view(irng));
        }

        //execute tests
        yarp::dev::tests::exec_iRangefinder2D_test_1(irng);

        //"Close all polydrivers and check"
        CHECK(laserdev.close());
    }

    Network::setLocalMode(false);
}
