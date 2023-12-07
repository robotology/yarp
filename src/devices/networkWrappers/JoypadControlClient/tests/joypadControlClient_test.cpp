/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/tests/IJoypadControllerTest.h>

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


TEST_CASE("dev::JoypadControlClientTest", "[yarp::dev]")
{
    #if defined(DISABLE_FAILING_TESTS)
        YARP_SKIP_TEST("Skipping failing tests")
    #endif

    YARP_REQUIRE_PLUGIN("fakeJoypad", "device");
    YARP_REQUIRE_PLUGIN("JoypadControlClient", "device");
    YARP_REQUIRE_PLUGIN("JoypadControlServer", "device");

    Network::setLocalMode(true);

    SECTION("Test the frameGrabber_nwc_yarp device with a frameGrabber_nws_yarp device")
    {
        PolyDriver dd_fake;
        PolyDriver dd_nws;
        PolyDriver dd_nwc;
        Property p_fake;
        Property p_nws;
        Property p_nwc;

        p_nws.put("device","JoypadControlServer");
        p_nws.put("use_separate_ports", true);
        p_nws.put("name", "/joyServer");
        p_fake.put("device","fakeJoypad");
        REQUIRE(dd_fake.open(p_fake));
        REQUIRE(dd_nws.open(p_nws));
        yarp::os::SystemClock::delaySystem(0.5);

        {yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
        REQUIRE(ww_nws);
        bool result_att = ww_nws->attach(&dd_fake);
        REQUIRE(result_att); }

        p_nwc.put("device", "JoypadControlClient");
        p_nwc.put("remote", "/joyServer");
        p_nwc.put("local", "/joyclient");
        REQUIRE(dd_nwc.open(p_nwc));

        IJoypadController* ijoy = nullptr;
        REQUIRE(dd_nwc.view(ijoy));

        yarp::os::SystemClock::delaySystem(0.5);

        yarp::dev::tests::exec_iJoypadController_test_1(ijoy);

        yarp::os::SystemClock::delaySystem(0.5);

        CHECK(dd_nwc.close());
        CHECK(dd_nws.close());
        CHECK(dd_fake.close());
    }

    Network::setLocalMode(false);
}
