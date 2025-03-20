/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/testInterfaces/IFakeDeviceInterfaceTest2.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeDeviceUnwrapped", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeDeviceUnwrapped", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeDeviceUnwrapped device")
    {
        PolyDriver dd;
        yarp::dev::test::IFakeDeviceInterfaceTest2* iTest=nullptr;

        ////////"Checking opening device polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeDeviceUnwrapped");
            REQUIRE(dd.open(p_cfg));
        }

        dd.view(iTest);
        REQUIRE(iTest);

        iTest->testSetValue(5);
        int val=0;
        iTest->testGetValue(val);
        CHECK(val==5);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
