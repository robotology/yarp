/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
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

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


TEST_CASE("dev::fakeDepthCameraTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeDepthCamera", "device");

    Network::setLocalMode(true);

    SECTION("Test the IFrameGrabberImage interface")
    {
        // Open the device
        PolyDriver dd;
        Property p;
        p.put("device", "fakeDepthCamera");
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
