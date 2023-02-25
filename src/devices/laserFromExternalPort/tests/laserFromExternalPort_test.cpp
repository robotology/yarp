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

TEST_CASE("dev::laserFromExternalPort", "[yarp::dev]")
{
    #if defined(DISABLE_FAILING_TESTS)
        YARP_SKIP_TEST("Skipping failing tests")
    #endif

    YARP_REQUIRE_PLUGIN("laserFromExternalPort", "device");
    YARP_REQUIRE_PLUGIN("fakeLaser", "device");
    YARP_REQUIRE_PLUGIN("rangefinder2D_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking laserFromExternalPort device")
    {
        PolyDriver laserdev;
        PolyDriver ddnws1;
        PolyDriver ddfake1;
        PolyDriver ddnws2;
        PolyDriver ddfake2;

        // open laser1
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "rangefinder2D_nws_yarp");
            pnws_cfg.put("period", "0.010");
            pnws_cfg.put("name", "/laser1");
            REQUIRE(ddnws1.open(pnws_cfg));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeLaser");
            REQUIRE(ddfake1.open(pdev_cfg));

            {yarp::dev::WrapperSingle* ww_nws; ddnws1.view(ww_nws);
            bool result_att = ww_nws->attach(&ddfake1);
            REQUIRE(result_att); }
        }
        // open laser2
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "rangefinder2D_nws_yarp");
            pnws_cfg.put("period", "0.010");
            pnws_cfg.put("name", "/laser2");
            REQUIRE(ddnws2.open(pnws_cfg));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeLaser");
            REQUIRE(ddfake2.open(pdev_cfg));

            {yarp::dev::WrapperSingle* ww_nws; ddnws2.view(ww_nws);
            bool result_att = ww_nws->attach(&ddfake2);
            REQUIRE(result_att); }
        }

        IRangefinder2D* irng = nullptr;

        ////////"Checking opening polydriver"
        {
            Property las_cfg;
            las_cfg.put("device", "laserFromExternalPort");
            Property& las_cfg_sensor = las_cfg.addGroup("SENSOR");
            las_cfg_sensor.put("input_ports_name", "\"(/laser1 /laser2)\"");
            las_cfg_sensor.put("min_angle", 0.0);
            las_cfg_sensor.put("max_angle", 360.0);
            las_cfg_sensor.put("resolution", 1.0);
            std::string las_cfg_str = las_cfg.toString();
            REQUIRE(laserdev.open(las_cfg));
            REQUIRE(laserdev.view(irng));
        }
        yarp::os::Network::connect ("/laser1", "/laserFromExternalPort:i");

        //execute tests
        yarp::dev::tests::exec_iRangefinder2D_test_1(irng);

        //"Close all polydrivers and check"
        CHECK(laserdev.close());
    }

    Network::setLocalMode(false);
}
