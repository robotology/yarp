/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IBattery.h>

#include <yarp/dev/tests/IBatteryTest.h>

#include <yarp/os/Network.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/os/Time.h>

#include <string>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


TEST_CASE("dev::battery_nwc_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeBattery", "device");
    YARP_REQUIRE_PLUGIN("battery_nwc_yarp", "device");
    YARP_REQUIRE_PLUGIN("battery_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Test the battery_nwc_yarp device with a battery_nws_yarp device")
    {
        PolyDriver dd_fake;
        PolyDriver dd_nws;
        PolyDriver dd_nwc;
        Property p_fake;
        Property p_nws;
        Property p_nwc;

        p_nws.put("device","battery_nws_yarp");
        p_nws.put("name", "/battery_nws");
        p_fake.put("device","fakeBattery");
        REQUIRE(dd_fake.open(p_fake));
        REQUIRE(dd_nws.open(p_nws));
        yarp::os::SystemClock::delaySystem(0.5);

        {yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
        REQUIRE(ww_nws);
        bool result_att = ww_nws->attach(&dd_fake);
        REQUIRE(result_att); }

        p_nwc.put("device", "battery_nwc_yarp");
        p_nwc.put("remote", "/battery_nws");
        p_nwc.put("local", "/battery_nwc");
        p_nwc.put("no_stream", 1);
        REQUIRE(dd_nwc.open(p_nwc));

        IBattery* ibattery = nullptr;
        REQUIRE(dd_nwc.view(ibattery));

        yarp::os::SystemClock::delaySystem(0.5);

        yarp::dev::tests::exec_iBattery_test_1(ibattery);

        yarp::os::SystemClock::delaySystem(0.5);

        CHECK(dd_nwc.close());
        CHECK(dd_nws.close());
        CHECK(dd_fake.close());
    }

    Network::setLocalMode(false);
}
