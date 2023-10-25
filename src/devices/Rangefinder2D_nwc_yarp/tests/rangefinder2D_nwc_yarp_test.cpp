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

TEST_CASE("dev::rangefinder2D_nwc_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeLaser", "device");
    YARP_REQUIRE_PLUGIN("rangefinder2D_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("rangefinder2D_nwc_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking rangefinder2D_nwc_yarp device")
    {
        PolyDriver ddlas;
        PolyDriver ddnws;
        PolyDriver ddnwc;
        IRangefinder2D* irng = nullptr;

        ////////"Checking opening polydriver"
        {
            Property plas_cfg;
            plas_cfg.put("device", "fakeLaser");
            plas_cfg.put("test","use_constant");
            plas_cfg.put("const_distance", 0.5);
            REQUIRE(ddlas.open(plas_cfg));
        }
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "rangefinder2D_nws_yarp");
            pnws_cfg.put("period", "0.010");
            pnws_cfg.put("name", "/laser");
            REQUIRE(ddnws.open(pnws_cfg));
        }

        //attach the nws to the fakelaser device
        {yarp::dev::WrapperSingle* ww_nws = nullptr; ddnws.view(ww_nws);
        REQUIRE(ww_nws);
        bool result_att = ww_nws->attach(&ddlas);
        REQUIRE(result_att); }

        //wait some time
        yarp::os::Time::delay(0.1);

        //create the client
        {
            Property pnwc_cfg;
            pnwc_cfg.put("device", "rangefinder2D_nwc_yarp");
            pnwc_cfg.put("local", "/local_laser");
            pnwc_cfg.put("remote", "/laser");
            REQUIRE(ddnwc.open(pnwc_cfg));
        }
        REQUIRE(ddnwc.view(irng));

        //execute tests
        yarp::dev::tests::exec_iRangefinder2D_test_1(irng);

        //Close all polydrivers and check
        CHECK(ddnwc.close());
        yarp::os::Time::delay(0.1);
        INFO("rangefinder2D_nwc_yarp closed");

        CHECK(ddnws.close());
        yarp::os::Time::delay(0.1);
        INFO("rangefinder2D_nws_yarp closed");

        CHECK(ddlas.close());
        yarp::os::Time::delay(0.1);
        INFO("fakeLaser closed");

        INFO("Test complete");
    }

    Network::setLocalMode(false);
}
