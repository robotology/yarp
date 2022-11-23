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

#include <string>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


TEST_CASE("dev::frameGrabber_nws_yarpTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeFrameGrabber", "device");
    YARP_REQUIRE_PLUGIN("frameGrabber_nwc_yarp", "device");
    YARP_REQUIRE_PLUGIN("frameGrabber_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Test the grabber wrapper")
    {
        PolyDriver dd;
        PolyDriver dd2;
        Property p;
        Property p2;
        p.put("device","frameGrabber_nwc_yarp");
        p.put("remote","/grabber");
        p.put("local","/grabber/client");

        p2.put("device","frameGrabber_nws_yarp");
        p2.put("subdevice","fakeFrameGrabber");

        REQUIRE(dd2.open(p2)); // server open reported successful
        REQUIRE(dd.open(p)); // client open reported successful

        IFrameGrabberImage *grabber = nullptr;
        REQUIRE(dd.view(grabber)); // interface reported

        yarp::os::SystemClock::delaySystem(0.5);

        ImageOf<PixelRgb> img;
        grabber->getImage(img);
        CHECK(img.width() > 0); // interface seems functional
        CHECK(dd2.close()); // server close reported successful
        CHECK(dd.close()); // client close reported successful
    }

    SECTION("Test the IRgbVisualParams interface")
    {
        // Try to open a FakeFrameGrabber and I check all the parameters
        PolyDriver dd;
        PolyDriver dd2;
        Property p;
        Property p2;
        Property intrinsics;
        Bottle* retM = nullptr;

        p.put("device","frameGrabber_nwc_yarp");
        p.put("remote","/grabber");
        p.put("local","/grabber/client");
        p.put("no_stream", 1);

        p2.put("device","frameGrabber_nws_yarp");
        p2.put("subdevice","fakeFrameGrabber");

        REQUIRE(dd2.open(p2)); // server open reported successful
        REQUIRE(dd.open(p)); // client open reported successful

        IFrameGrabberImage* igrabber = nullptr;
        IRgbVisualParams* irgbParams = nullptr;
        REQUIRE(dd.view(igrabber)); // interface rgb params reported
        REQUIRE(dd.view(irgbParams)); // interface rgb params reported

        yarp::dev::tests::exec_IFrameGrabberImage_test_1(igrabber);
        yarp::dev::tests::exec_IRgbVisualParams_test_1(irgbParams);

        // Test the crop function - must work.
        IFrameGrabberImage *grabber = nullptr;
        REQUIRE(dd.view(grabber));
        ImageOf<PixelRgb> img;
        ImageOf<PixelRgb> crop;
        grabber->getImage(img);

        yarp::sig::VectorOf<std::pair< int, int>> vertices;
        vertices.resize(2);
        vertices[0] = std::pair <int, int> (0, 0);
        vertices[1] = std::pair <int, int> (10, 10); // Configure a doable crop.

        // check crop function
        CHECK(grabber->getImageCrop(YARP_CROP_RECT, vertices, crop));
        CHECK(crop.width() > 0);
        CHECK(crop.height() > 0);

        CHECK(dd2.close()); // server close reported successful
        CHECK(dd.close()); // client close reported successful
    }

    Network::setLocalMode(false);
}
