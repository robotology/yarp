/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRangefinder2D.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IRangefinder2DTest.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::FakeLaserTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeLaser", "device");

    Network::setLocalMode(true);

    SECTION("Checking FakeLaserTest device")
    {
        PolyDriver fakelaserdev;
        IRangefinder2D* irng = nullptr;

        ////////"Checking opening polydriver"
        {
            Property las_cfg;
            las_cfg.put("device", "fakeLaser");
            las_cfg.put("test", "use_constant");
            las_cfg.put("const_distance", 0.5);
            REQUIRE(fakelaserdev.open(las_cfg));
            REQUIRE(fakelaserdev.view(irng));
        }

        //execute tests
        yarp::dev::tests::exec_iRangefinder2D_test_1(irng);

        //"Close all polydrivers and check"
        CHECK(fakelaserdev.close());
    }

    Network::setLocalMode(false);
}
