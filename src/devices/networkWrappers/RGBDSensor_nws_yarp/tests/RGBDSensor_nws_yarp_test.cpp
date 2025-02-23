/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/tests/IRGBDSensorTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::RGBDSensor_nws_yarpTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeDepthCamera", "device");
    YARP_REQUIRE_PLUGIN("RGBDSensor_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking RGBDSensor_nws_yarp device")
    {
        PolyDriver nws_driver;

        ////////"Checking opening polydriver"
        {
            Property nws_cfg;
            nws_cfg.put("device", "RGBDSensor_nws_yarp");
            nws_cfg.put("name", "/rgbd_nws");
            REQUIRE(nws_driver.open(nws_cfg));
        }

        //Close all polydrivers and check
        CHECK(nws_driver.close());
    }

    SECTION("Test the RGBDSensor_nws_yarp device with fakeDepthCamera device attached")
    {
        std::vector<std::string> fakeSens;
        fakeSens.push_back("fakeDepthCamera");
        fakeSens.push_back("fakeDepthCamera_mini");

        for (auto it = fakeSens.begin(); it != fakeSens.end(); it++)
        {
            PolyDriver dd_fake;
            PolyDriver dd_nws;
            Property p_fake;
            Property p_nws;

            // open
            p_nws.put("device", "RGBDSensor_nws_yarp");
            p_nws.put("name", "/rgbd_nws");
            p_fake.put("device", *it);
            // small values to improve valgrind speed
            p_fake.put("rgb_w", 32);
            p_fake.put("rgb_h", 24);
            p_fake.put("dep_w", 32);
            p_fake.put("dep_h", 24);
            REQUIRE(dd_fake.open(p_fake));
            REQUIRE(dd_nws.open(p_nws));

            yarp::os::SystemClock::delaySystem(0.5);

            // attach
            {
                yarp::dev::WrapperSingle* ww_nws;
                dd_nws.view(ww_nws);
                REQUIRE(ww_nws);
                bool result_att = ww_nws->attach(&dd_fake);
                REQUIRE(result_att);
            }

            // Wait some time
            yarp::os::SystemClock::delaySystem(1.0);

            // Close all polydrivers and check
            {
                CHECK(dd_nws.close());
                CHECK(dd_fake.close());
            }
        }
    }

    Network::setLocalMode(false);
}
