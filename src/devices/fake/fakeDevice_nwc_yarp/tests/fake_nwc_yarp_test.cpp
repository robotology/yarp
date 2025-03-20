/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/testInterfaces/IFakeDeviceInterfaceTest1.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeDevice_nwc_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeDeviceUnwrapped", "device");
    YARP_REQUIRE_PLUGIN("fakeDevice_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("fakeDevice_nwc_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking Fake_nwc device")
    {
        yarp::dev::test::IFakeDeviceInterfaceTest1* ifake=nullptr;
        PolyDriver ddnws;
        PolyDriver ddfake;
        PolyDriver ddnwc;

        ////////"Checking opening LLM_nws_yarp and LLM_nwc_yarp polydrivers"
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "fakeDevice_nws_yarp");
            REQUIRE(ddnws.open(pnws_cfg));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeDeviceUnwrapped");
            REQUIRE(ddfake.open(pdev_cfg));

            {yarp::dev::WrapperSingle* ww_nws=nullptr; ddnws.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&ddfake);
            REQUIRE(result_att); }

            Property pnwc_cfg;
            pnwc_cfg.put("device", "fakeDevice_nwc_yarp");
            REQUIRE(ddnwc.open(pnwc_cfg));
            REQUIRE(ddnwc.view(ifake));
        }

        //Test the interface
        ReturnValue ret = ifake->doSomething();
        CHECK(ret);

        //"Close all polydrivers and check"
        {
            CHECK(ddnwc.close());
            yarp::os::Time::delay(0.1);
            CHECK(ddnws.close());
            yarp::os::Time::delay(0.1);
            CHECK(ddfake.close());
        }
    }

    Network::setLocalMode(false);
}
