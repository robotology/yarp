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

#include <array>
#include <string>

#include <catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


TEST_CASE("dev::fakeFrameGrabberTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeFrameGrabber", "device");

    Network::setLocalMode(true);

    SECTION("Test the IFrameGrabberImage interface")
    {
        // Open the device
        PolyDriver dd;
        Property p;
        p.put("device", "fakeFrameGrabber");
        REQUIRE(dd.open(p));

        // Get the IFrameGrabberImage interface
        IFrameGrabberImage* iFrameGrabberImage = nullptr;
        REQUIRE(dd.view(iFrameGrabberImage));

        yarp::dev::tests::exec_IFrameGrabberImage_test_1(iFrameGrabberImage);

        // Close the device
        CHECK(dd.close());
    }


    SECTION("Test the IRgbVisualParams interface")
    {
        // Open the device
        PolyDriver dd;
        Property p;
        p.put("device","fakeFrameGrabber");
        REQUIRE(dd.open(p));

        // Get the IRgbVisualParams interface
        IRgbVisualParams* rgbParams = nullptr;
        REQUIRE(dd.view(rgbParams));

        yarp::dev::tests::exec_IRgbVisualParams_test_1(rgbParams);

        // Close the device
        CHECK(dd.close()); // client close reported successful
    }

    // TODO Add tests for the other interfaces

    Network::setLocalMode(false);
}
