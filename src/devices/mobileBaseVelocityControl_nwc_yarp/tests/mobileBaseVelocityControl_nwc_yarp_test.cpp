/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::mobileBaseVelocityControl_nws_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeNavigation", "device");
    YARP_REQUIRE_PLUGIN("mobileBaseVelocityControl_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("mobileBaseVelocityControl_nwc_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking mobileBaseVelocityControl_nwc_yarp device")
    {
        PolyDriver ddnws;
        PolyDriver ddfake;
        PolyDriver ddnwc;
        yarp::dev::Nav2D::INavigation2DVelocityActions* ivel = nullptr;

        ////////"Checking opening localization2D_nws_yarp polydrivers"
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "mobileBaseVelocityControl_nws_yarp");
            pnws_cfg.put("local",  "/mobileBaseVelocityControl_nws_yarp");
            REQUIRE(ddnws.open(pnws_cfg));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeNavigation");
            REQUIRE(ddfake.open(pdev_cfg));

            {yarp::dev::WrapperSingle* ww_nws; ddnws.view(ww_nws);
            bool result_att = ww_nws->attach(&ddfake);
            REQUIRE(result_att); }

            Property pclient_cfg;
            pclient_cfg.put("device", "mobileBaseVelocityControl_nwc_yarp");
            pclient_cfg.put("local",  "/mobileBaseVelocityControl_nwc_yarp");
            pclient_cfg.put("server", "/mobileBaseVelocityControl_nws_yarp");
            REQUIRE(ddnwc.open(pclient_cfg));
            REQUIRE(ddnwc.view(ivel));
        }

        yarp::os::Time::delay(0.1);

        CHECK (ivel->applyVelocityCommand(1,2,3,10));
        double x=0; double y=0; double t=0;
        CHECK( ivel->getLastVelocityCommand(x,y,t));
        CHECK(x == 1);
        CHECK(y == 2);
        CHECK(t == 3);

        //"Close all polydrivers and check"
        {
            CHECK(ddnwc.close());
            CHECK(ddnws.close());
            CHECK(ddfake.close());
        }
    }

    Network::setLocalMode(false);
}
