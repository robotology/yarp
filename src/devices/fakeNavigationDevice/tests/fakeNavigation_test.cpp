/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/INavigation2DTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::FakeNavigationTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeNavigation", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeNavigation device")
    {
        PolyDriver ddfakeNavigation;
        INavigation2DTargetActions* inav_target = nullptr;
        INavigation2DControlActions* inav_control = nullptr;
        INavigation2DVelocityActions* inav_velocity = nullptr;

        ////////"Checking opening fakeNavigation polydrivers"
        {
            Property pfakeNavigation;
            pfakeNavigation.put("device", "fakeNavigation");
            REQUIRE(ddfakeNavigation.open(pfakeNavigation));
            REQUIRE(ddfakeNavigation.view(inav_target));
            REQUIRE(inav_target);
            REQUIRE(ddfakeNavigation.view(inav_control));
            REQUIRE(inav_control);
            REQUIRE(ddfakeNavigation.view(inav_velocity));
            REQUIRE(inav_velocity);
        }

        // Do tests
        yarp::dev::tests::exec_iNav2D_test_3(inav_target, inav_control);

        //"Close all polydrivers and check"
        {
            CHECK(ddfakeNavigation.close());
        }
    }

    Network::setLocalMode(false);
}
