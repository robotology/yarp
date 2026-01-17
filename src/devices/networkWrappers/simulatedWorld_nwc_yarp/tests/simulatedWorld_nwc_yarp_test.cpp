/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ISimulatedWorld.h>

//#include <yarp/dev/tests/ISimulatedWorldTest.h>

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


TEST_CASE("dev::SimulatedWorld_nwc_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeSimulatedWorld", "device");
    YARP_REQUIRE_PLUGIN("simulatedWorld_nwc_yarp", "device");
    YARP_REQUIRE_PLUGIN("simulatedWorld_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Test the simulatedWorld_nwc_yarp device with a simulatedWorld_nws_yarp device")
    {
        PolyDriver dd_fake;
        PolyDriver dd_nws;
        PolyDriver dd_nwc;
        Property p_fake;
        Property p_nws;
        Property p_nwc;

        p_nws.put("device","simulatedWorld_nws_yarp");
        p_nws.put("name", "/simulatedWorld_nws");
        p_fake.put("device","fakeSimulatedWorld");
        REQUIRE(dd_fake.open(p_fake));
        REQUIRE(dd_nws.open(p_nws));
        yarp::os::SystemClock::delaySystem(0.5);

        {yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
        REQUIRE(ww_nws);
        bool result_att = ww_nws->attach(&dd_fake);
        REQUIRE(result_att); }

        p_nwc.put("device", "simulatedWorld_nwc_yarp");
        p_nwc.put("remote", "/simulatedWorld_nws");
        p_nwc.put("local", "/simulatedWorld_nwc");
        REQUIRE(dd_nwc.open(p_nwc));

        ISimulatedWorld* isim = nullptr;
        REQUIRE(dd_nwc.view(isim));

        yarp::os::SystemClock::delaySystem(0.5);

        isim->makeSphere("sphere1", 0.1, yarp::sig::Pose6D(), yarp::sig::ColorRGB(1.0, 0.0, 0.0));
        //yarp::dev::tests::exec_iBattery_test_1(isim);

        yarp::os::SystemClock::delaySystem(0.5);

        CHECK(dd_nwc.close());
        CHECK(dd_nws.close());
        CHECK(dd_fake.close());
    }

    Network::setLocalMode(false);
}
