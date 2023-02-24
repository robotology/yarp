/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IOdometry2D.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IOdometry2DTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeOdometry2D", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeOdometry2D", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeOdometry2D device")
    {
        PolyDriver dd;
        yarp::dev::Nav2D::IOdometry2D* iodom=nullptr;

        ////////"Checking opening device polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeOdometry2D");
            REQUIRE(dd.open(p_cfg));
        }

        dd.view(iodom);
        yarp::dev::tests::exec_iOdometry2D_test_1(iodom);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
