/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ILLM.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/ILLMTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::LLM_nwc_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeLLMDevice", "device");
    YARP_REQUIRE_PLUGIN("LLM_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("LLM_nwc_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking LLM_nwc device")
    {
        yarp::dev::ILLM* illm=nullptr;
        PolyDriver ddnws;
        PolyDriver ddfake;
        PolyDriver ddnwc;

        ////////"Checking opening LLM_nws_yarp and LLM_nwc_yarp polydrivers"
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "LLM_nws_yarp");
            pnws_cfg.put("name", "/LLM_nws");
            REQUIRE(ddnws.open(pnws_cfg));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeLLMDevice");
            REQUIRE(ddfake.open(pdev_cfg));

            {yarp::dev::WrapperSingle* ww_nws=nullptr; ddnws.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&ddfake);
            REQUIRE(result_att); }

            Property pnwc_cfg;
            pnwc_cfg.put("device", "LLM_nwc_yarp");
            pnwc_cfg.put("local", "/LLM_nwc/rpc");
            pnwc_cfg.put("remote", "/LLM_nws/rpc:i");
            REQUIRE(ddnwc.open(pnwc_cfg));
            REQUIRE(ddnwc.view(illm));
        }

        yarp::dev::tests::exec_iLLM_test_1(illm);

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
