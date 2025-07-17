/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/tests/IRGBDSensorTest.h>

#include <yarp/os/Network.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>

#include <array>
#include <string>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


TEST_CASE("dev::fakeDepthCamera_miniTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeDepthCamera_mini", "device");

    Network::setLocalMode(true);

    SECTION("Test the IFrameGrabberImage interface")
    {
        // Open the device
        PolyDriver dd;
        Property p;
        p.put("device", "fakeDepthCamera_mini");
        // small values to improve valgrind speed
        p.put("rgb_w", 32);
        p.put("rgb_h", 24);
        p.put("dep_w", 32);
        p.put("dep_h", 24);
        REQUIRE(dd.open(p));

        // Get the IFrameGrabberImage interface
        IRGBDSensor* irgbd = nullptr;
        REQUIRE(dd.view(irgbd));

        yarp::dev::tests::exec_iRGBDSensor_test_1(irgbd);

        // Close the device
        CHECK(dd.close());
    }

    Network::setLocalMode(false);
}
