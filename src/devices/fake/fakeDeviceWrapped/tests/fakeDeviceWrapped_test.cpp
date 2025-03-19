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

TEST_CASE("dev::fakeDeviceWrapped", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeDeviceWrapped", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeDeviceWrapped device")
    {
        PolyDriver dd;
        yarp::dev::test::IFakeDeviceInterfaceTest2* iTest = nullptr;

        ////////"Checking opening device polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeDeviceWrapped");
            REQUIRE(dd.open(p_cfg));
        }

        dd.view(iTest);
        REQUIRE(iTest);

        iTest->testSetValue(5);
        int val = 0;
        iTest->testGetValue(val);
        CHECK(val == 5);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    SECTION("Checking fakeDeviceWrapped device with automatically invoked wrapper")
    {
        PolyDriver dd;
        yarp::dev::test::IFakeDeviceInterfaceTest2* iTest = nullptr;

        ////////"Checking opening device polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeDeviceWrapped");
            //this option, should be used only by yarpdev executable: it is used here for testing purposes only,
            //wrapping_enabled=1 will open deviceBundler, fakeDeviceWrapper and perform an attach with fakeDeviceWrapped.
            p_cfg.put("wrapping_enabled", 1);
            REQUIRE(dd.open(p_cfg));
        }

        //what interfaces are available now?
        //This part is not well specified.
        //Currently no interfaces can be opened since dd will be a deviceBundler and not
        //a fakeDeviceWrapped (even if dd.id() shows `fakeDeviceWrapped`).
        //This behavior may change in the future, but it is not very important since `wrapping_enabled` is used
        //only by yarpdev executable.
        std::string devicename = dd.id();
        CHECK (devicename == "fakeDeviceWrapped");

        REQUIRE(iTest == nullptr);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
