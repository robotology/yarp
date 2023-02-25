/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IRgbVisualParams.h>

#include <yarp/dev/tests/IFrameGrabberImageTest.h>
#include <yarp/dev/tests/IRgbVisualParamsTest.h>

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


TEST_CASE("dev::battery_nws_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeBattery", "device");
    YARP_REQUIRE_PLUGIN("battery_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Test the battery_nws_yarp device with no device attached")
    {
        PolyDriver dd_nws;
        Property p_nws;

        p_nws.put("device","battery_nws_yarp");
        p_nws.put("name", "/battery");
        REQUIRE(dd_nws.open(p_nws));
        yarp::os::SystemClock::delaySystem(1.0);
        CHECK(dd_nws.close());
    }

    SECTION("Test the battery_nws_yarp device with fakeBattery device attached")
    {
        PolyDriver dd_fake;
        PolyDriver dd_nws;
        Property p_fake;
        Property p_nws;

        p_nws.put("device", "battery_nws_yarp");
        p_nws.put("name", "/battery");
        p_fake.put("device", "fakeBattery");
        REQUIRE(dd_fake.open(p_fake));
        REQUIRE(dd_nws.open(p_nws));
        yarp::os::SystemClock::delaySystem(0.5);

        {yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
        REQUIRE(ww_nws);
        bool result_att = ww_nws->attach(&dd_fake);
        REQUIRE(result_att); }

        yarp::os::SystemClock::delaySystem(1.0);

        CHECK(dd_nws.close());
        CHECK(dd_fake.close());
    }

    Network::setLocalMode(false);
}
