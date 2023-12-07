/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IOdometry2D.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::odometry2d_nws_yarpTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeOdometry2D", "device");
    YARP_REQUIRE_PLUGIN("odometry2D_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking odometry2D_nws_yarp device alone")
    {
        PolyDriver nws_driver;
        yarp::dev::Nav2D::IOdometry2D* iodom = nullptr;

        ////////"Checking opening polydriver"
        {
            Property nws_cfg;
            nws_cfg.put("device", "odometry2D_nws_yarp");
            nws_cfg.put("period", "0.010");
            nws_cfg.put("name", "/laser");
            REQUIRE(nws_driver.open(nws_cfg));
        }

        //Close all polydrivers and check
        CHECK(nws_driver.close());
    }

    SECTION("Checking odometry2D_nws_yarp device attached to fakeOdometry2D")
    {
        PolyDriver ddnws;
        PolyDriver ddfake;

        ////////"Checking opening rangefinder2D_nws_yarp polydrivers"
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "odometry2D_nws_yarp");
            pnws_cfg.put("period", "0.010");
            pnws_cfg.put("name", "/laser");
            REQUIRE(ddnws.open(pnws_cfg));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeOdometry2D");
            REQUIRE(ddfake.open(pdev_cfg));

            {yarp::dev::WrapperSingle* ww_nws=nullptr; ddnws.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&ddfake);
            REQUIRE(result_att); }
        }

        yarp::os::Time::delay(0.1);

        //"Close all polydrivers and check"
        {
            CHECK(ddnws.close());
            CHECK(ddfake.close());
        }
    }

    Network::setLocalMode(false);
}
