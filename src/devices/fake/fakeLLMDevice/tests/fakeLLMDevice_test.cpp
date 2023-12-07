/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/ILLM.h>
#include <yarp/dev/tests/ILLMTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeLLMDeviceTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeLLMDevice", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeLLMdevice")
    {
        PolyDriver fakellmdev;
        ILLM* illm = nullptr;

        ////////"Checking opening polydriver"
        {
            Property llm_cfg;
            llm_cfg.put("device", "fakeLLMDevice");
            REQUIRE(fakellmdev.open(llm_cfg));
            REQUIRE(fakellmdev.view(illm));
        }

        //execute tests
        yarp::dev::tests::exec_iLLM_test_1(illm);

        //"Close all polydrivers and check"
        CHECK(fakellmdev.close());
    }

    Network::setLocalMode(false);
}
