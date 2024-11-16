/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/ILocalization2D.h>
#include <yarp/dev/tests/ILocalization2DTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeLocalizer", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeLocalizer", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeLocalizer device")
    {
        PolyDriver dd;
        yarp::dev::Nav2D::ILocalization2D* iloc=nullptr;

        ////////"Checking opening polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeLocalizer");
            REQUIRE(dd.open(p_cfg));
        }

        dd.view(iloc);
        yarp::dev::tests::exec_iLocalization2D_test_1(iloc);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
