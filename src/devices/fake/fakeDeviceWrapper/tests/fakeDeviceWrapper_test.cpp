/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/testInterfaces/IFakeDeviceInterfaceTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeDeviceWrapper", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeDeviceUnwrapped", "device");
    YARP_REQUIRE_PLUGIN("fakeDeviceWrapper", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeDeviceWrapper device alone")
    {
        PolyDriver dd;
        yarp::dev::test::IFakeDeviceInterfaceTest* iTest = nullptr;

        ////////"Checking opening device polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeDeviceWrapper");
            REQUIRE(dd.open(p_cfg));
        }

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    SECTION("Checking fakeDeviceWrapped device with automatically invoked wrapper")
    {
        PolyDriver dd;
        PolyDriver dd_wrapper;
        yarp::dev::test::IFakeDeviceInterfaceTest* iTest1 = nullptr;
        yarp::dev::test::IFakeDeviceInterfaceTest* iTest2 = nullptr;

        ////////"Checking opening device polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeDeviceUnwrapped");
            REQUIRE(dd.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "fakeDeviceWrapper");
            REQUIRE(dd_wrapper.open(p_cfg));
        }
        {
            yarp::dev::WrapperSingle* ww_wrap = nullptr;
            dd_wrapper.view(ww_wrap);
            REQUIRE(ww_wrap);
            bool result_att = ww_wrap->attach(&dd);
            REQUIRE(result_att);
        }

        dd.view(iTest1);
        REQUIRE(iTest1);
        dd_wrapper.view(iTest2);
        REQUIRE(iTest2);

        iTest1->testSetValue(5);
        int val = 0;
        iTest1->testGetValue(val);
        CHECK(val == 5);

        iTest2->testSetValue(6);
        iTest2->testGetValue(val);
        CHECK(val == 6);

        iTest1->testSetValue(3);
        iTest2->testGetValue(val);
        CHECK(val == 3);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
