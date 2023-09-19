/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::controlBoard_nws_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("controlBoard_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking controlBoard_nws_yarp device")
    {
        PolyDriver ddnws;

        ////////"Checking opening map2DServer and map2DClient polydrivers"
        {
            Property pcfg;
            pcfg.put("device", "controlBoard_nws_yarp");
            pcfg.put("name", "/controlboard");
            REQUIRE(ddnws.open(pcfg));
        }

        //"Close all polydrivers and check"
        {
            CHECK(ddnws.close());
        }
    }

    SECTION("Test the controlBoard_nws_yarp device with fakeMotionControl device attached")
    {
        PolyDriver dd_fake;
        PolyDriver dd_nws;
        Property p_fake;
        Property p_nws;

        //open
        p_nws.put("device", "controlBoard_nws_yarp");
        p_nws.put("name", "/controlboard");
        p_fake.put("device", "fakeMotionControl");
        REQUIRE(dd_fake.open(p_fake));
        REQUIRE(dd_nws.open(p_nws));

        yarp::os::SystemClock::delaySystem(0.5);

        //attach
        {
            yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&dd_fake);
            REQUIRE(result_att);
        }

        yarp::os::SystemClock::delaySystem(1.0);

        //Close all polydrivers and check
        {
            CHECK(dd_nws.close());
            CHECK(dd_fake.close());
        }
    }

    Network::setLocalMode(false);
}
