/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IChatBot.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IChatBotTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::ChatBot_nwc", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeChatBotDevice", "device");
    YARP_REQUIRE_PLUGIN("chatBot_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("chatBot_nwc_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking chatBot_nwc device")
    {
        yarp::dev::IChatBot* iChatBot=nullptr;
        PolyDriver ddnws;
        PolyDriver ddfake;
        PolyDriver ddnwc;

        ////////"Checking opening ChatBot_nws_yarp and ChatBot_nwc_yarp polydrivers"
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "chatBot_nws_yarp");
            pnws_cfg.put("name", "/chatBot_nws");
            REQUIRE(ddnws.open(pnws_cfg));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeChatBotDevice");
            REQUIRE(ddfake.open(pdev_cfg));

            {yarp::dev::WrapperSingle* ww_nws; ddnws.view(ww_nws);
            bool result_att = ww_nws->attach(&ddfake);
            REQUIRE(result_att); }

            Property pnwc_cfg;
            pnwc_cfg.put("device", "chatBot_nwc_yarp");
            pnwc_cfg.put("local", "/chatBot_nwc/rpc");
            pnwc_cfg.put("remote", "/chatBot_nws/rpc");
            REQUIRE(ddnwc.open(pnwc_cfg));
            REQUIRE(ddnwc.view(iChatBot));
        }

        yarp::dev::tests::exec_iChatBot_test_1(iChatBot);

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
