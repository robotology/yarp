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
#include <yarp/os/Time.h>
#include <yarp/dev/WrapperSingle.h>

#include <string>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


TEST_CASE("dev::joypadControlServerTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeJoypad", "device");
    YARP_REQUIRE_PLUGIN("JoypadControlServer", "device");

    Network::setLocalMode(true);

    SECTION("Test the nws alone")
    {
        PolyDriver dd_nws;
        Property p_nws;

        p_nws.put("device", "JoypadControlServer");
        p_nws.put("use_separate_ports", true);
        p_nws.put("name", "/joyServer");

        REQUIRE(dd_nws.open(p_nws));
        yarp::os::SystemClock::delaySystem(0.5);

        CHECK(dd_nws.close());
    }

    SECTION("Test the nws attached to the fake device")
    {
        PolyDriver dd_fake;
        PolyDriver dd_nws;
        Property p_fake;
        Property p_nws;

        p_nws.put("device", "JoypadControlServer");
        p_nws.put("use_separate_ports", true);
        p_nws.put("name", "/joyServer");

        p_fake.put("device", "fakeJoypad");

        REQUIRE(dd_fake.open(p_fake));
        REQUIRE(dd_nws.open(p_nws));
        yarp::os::SystemClock::delaySystem(0.5);

        {yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
        REQUIRE(ww_nws);
        bool result_att = ww_nws->attach(&dd_fake);
        REQUIRE(result_att); }

        yarp::os::SystemClock::delaySystem(0.5);

        CHECK(dd_nws.close());
        CHECK(dd_fake.close());
    }

    Network::setLocalMode(false);
}
