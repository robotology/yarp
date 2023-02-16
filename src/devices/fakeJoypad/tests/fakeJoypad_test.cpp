/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IJoypadController.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IJoypadControllerTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::fakeJoypad", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeJoypad", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeJoypad device")
    {
        PolyDriver fakedev;
        IJoypadController* ijoy = nullptr;

        ////////"Checking opening polydriver"
        {
            Property cfg;
            cfg.put("device", "fakeJoypad");
            REQUIRE(fakedev.open(cfg));
            REQUIRE(fakedev.view(ijoy));
        }

        //execute tests
        yarp::dev::tests::exec_iJoypadController_test_1(ijoy);

        //"Close all polydrivers and check"
        CHECK(fakedev.close());
    }

    Network::setLocalMode(false);
}
