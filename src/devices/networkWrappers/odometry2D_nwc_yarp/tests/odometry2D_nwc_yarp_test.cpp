/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
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

TEST_CASE("dev::odometry2D_nwc_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeOdometry2D", "device");
    YARP_REQUIRE_PLUGIN("odometry2D_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("odometry2D_nwc_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking odometry2D_nwc_yarp device")
    {
        PolyDriver ddlas;
        PolyDriver ddnws;
        PolyDriver ddnwc;
        Nav2D::IOdometry2D* iodom = nullptr;

        ////////"Checking opening polydriver"
        {
            Property plas_cfg;
            plas_cfg.put("device", "fakeOdometry2D");
            REQUIRE(ddlas.open(plas_cfg));
        }
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "odometry2D_nws_yarp");
            pnws_cfg.put("period", 0.010);
            pnws_cfg.put("name", "/odom");
            REQUIRE(ddnws.open(pnws_cfg));
        }

        //attach the nws to the fakeOdometry2D device
        {yarp::dev::WrapperSingle* ww_nws = nullptr; ddnws.view(ww_nws);
        REQUIRE(ww_nws);
        bool result_att = ww_nws->attach(&ddlas);
        REQUIRE(result_att); }

        //wait some time
        yarp::os::Time::delay(0.1);

        //create the client
        {
            Property pnwc_cfg;
            pnwc_cfg.put("device", "odometry2D_nwc_yarp");
            pnwc_cfg.put("local", "/local_odom");
            pnwc_cfg.put("remote", "/odom");
            REQUIRE(ddnwc.open(pnwc_cfg));
        }
        REQUIRE(ddnwc.view(iodom));

        //execute tests
        yarp::dev::tests::exec_iOdometry2D_test_1(iodom);

        //Close all polydrivers and check
        CHECK(ddnwc.close());
        yarp::os::Time::delay(0.1);
        INFO("odometry2D_nwc_yarp closed");

        CHECK(ddnws.close());
        yarp::os::Time::delay(0.1);
        INFO("odometry2D_nws_yarp closed");

        CHECK(ddlas.close());
        yarp::os::Time::delay(0.1);
        INFO("fakeOdometry2D closed");

        INFO("Test complete");
    }

    Network::setLocalMode(false);
}
