/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/IChatBot.h>
#include <yarp/dev/tests/IChatBotTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeChatBotDeviceTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeChatBotDevice", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeChatBotdevice")
    {
        PolyDriver fakeChatBotdev;
        IChatBot* iChatBot = nullptr;

        ////////"Checking opening polydriver"
        {
            Property chatBot_cfg;
            chatBot_cfg.put("device", "fakeChatBotDevice");
            REQUIRE(fakeChatBotdev.open(chatBot_cfg));
            REQUIRE(fakeChatBotdev.view(iChatBot));
        }

        //execute tests
        yarp::dev::tests::exec_iChatBot_test_1(iChatBot);

        //"Close all polydrivers and check"
        CHECK(fakeChatBotdev.close());
    }

    Network::setLocalMode(false);
}
