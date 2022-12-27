/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/tests/IRGBDSensorTest.h>

#include <catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::Rangefinder2D_nws_yarpTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeDepthCamera", "device");
    YARP_REQUIRE_PLUGIN("rgbdSensor_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking rgbdSensor_nws_yarp device")
    {
        PolyDriver nws_driver;

        ////////"Checking opening polydriver"
        {
            Property nws_cfg;
            nws_cfg.put("device", "rgbdSensor_nws_yarp");
            nws_cfg.put("name", "/rgbd_nws");
            REQUIRE(nws_driver.open(nws_cfg));
        }

        //Close all polydrivers and check
        CHECK(nws_driver.close());
    }

    Network::setLocalMode(false);
}
