/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
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


TEST_CASE("dev::simulatedWorld_nws_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeSimulatedWorld", "device");
    YARP_REQUIRE_PLUGIN("simulatedWorld_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Test the simulatedWorld_nws_yarp device with no device attached")
    {
        PolyDriver dd_nws;
        Property p_nws;

        p_nws.put("device","simulatedWorld_nws_yarp");
        p_nws.put("name", "/sim_nws");
        REQUIRE(dd_nws.open(p_nws));
        yarp::os::SystemClock::delaySystem(1.0);
        CHECK(dd_nws.close());
    }

    SECTION("Test the simulatedWorld_nws_yarp device with fakeSimulatedWorld device attached")
    {
        PolyDriver dd_fake;
        PolyDriver dd_nws;
        Property p_fake;
        Property p_nws;

        p_nws.put("device", "simulatedWorld_nws_yarp");
        p_nws.put("name", "/sim_nws");
        p_fake.put("device", "fakeSimulatedWorld");
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
