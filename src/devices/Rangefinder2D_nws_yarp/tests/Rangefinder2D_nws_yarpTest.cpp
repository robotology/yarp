/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRangefinder2D.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IRangefinder2DTest.h>

#include <catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::Rangefinder2D_nws_yarpTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("rangefinder2D_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking Rangefinder2D_nws_yarp device")
    {
        PolyDriver nws_driver;
        IRangefinder2D* irng = nullptr;

        ////////"Checking opening polydriver"
        {
            Property nws_cfg;
            nws_cfg.put("device", "rangefinder2D_nws_yarp");
            nws_cfg.put("period", "0.010");
            nws_cfg.put("name", "/laser");
            REQUIRE(nws_driver.open(nws_cfg));
        }

        //Close all polydrivers and check
        CHECK(nws_driver.close());
    }

    Network::setLocalMode(false);
}
