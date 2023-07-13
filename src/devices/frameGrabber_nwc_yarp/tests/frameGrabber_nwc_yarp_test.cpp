/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
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

void do_nws_nwc_test(bool use_stream)
{
    PolyDriver dd_fake;
    PolyDriver dd_nws;
    PolyDriver dd_nwc;
    Property p_fake;
    Property p_nws;
    Property p_nwc;

    p_nws.put("device", "frameGrabber_nws_yarp");
    p_fake.put("device", "fakeFrameGrabber");
    REQUIRE(dd_fake.open(p_fake));
    REQUIRE(dd_nws.open(p_nws));
    yarp::os::SystemClock::delaySystem(0.5);

    {yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
    REQUIRE(ww_nws);
    bool result_att = ww_nws->attach(&dd_fake);
    REQUIRE(result_att); }

    p_nwc.put("device", "frameGrabber_nwc_yarp");
    p_nwc.put("remote", "/grabber");
    p_nwc.put("local", "/grabber/client");
    if (!use_stream) {p_nwc.put("no_stream",true);}
    REQUIRE(dd_nwc.open(p_nwc));

    IFrameGrabberImage* igrabber = nullptr;
    IRgbVisualParams* irgbParams = nullptr;
    REQUIRE(dd_nwc.view(igrabber));
    REQUIRE(dd_nwc.view(irgbParams));

    yarp::os::SystemClock::delaySystem(0.5);
    {
        ImageOf<PixelRgb> img;
        CHECK(img.width() == 0);
        CHECK(img.height() == 0);
        igrabber->getImage(img);
        CHECK(img.width() > 0);
        CHECK(img.height() > 0);
    }
    yarp::os::SystemClock::delaySystem(0.5);
    {
        ImageOf<PixelRgb> crop;
        yarp::sig::VectorOf<std::pair< int, int>> vertices;
        vertices.resize(2);
        vertices[0] = std::pair <int, int>(0, 0);
        vertices[1] = std::pair <int, int>(10, 10); // Configure a doable crop.
        CHECK(crop.width() == 0);
        CHECK(crop.height() == 0);
        CHECK(igrabber->getImageCrop(YARP_CROP_RECT, vertices, crop));
        CHECK(crop.width() > 0);
        CHECK(crop.height() > 0);
    }

    yarp::dev::tests::exec_IFrameGrabberImage_test_1(igrabber);
    yarp::dev::tests::exec_IRgbVisualParams_test_1(irgbParams);

    yarp::os::SystemClock::delaySystem(0.5);

    CHECK(dd_nwc.close());
    CHECK(dd_nws.close());
    CHECK(dd_fake.close());
}

TEST_CASE("dev::frameGrabber_nwc_yarpTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeFrameGrabber", "device");
    YARP_REQUIRE_PLUGIN("frameGrabber_nwc_yarp", "device");
    YARP_REQUIRE_PLUGIN("frameGrabber_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Test the frameGrabber_nwc_yarp device with a frameGrabber_nws_yarp device (RPC mode)")
    {
        do_nws_nwc_test(false);
    }

    SECTION("Test the frameGrabber_nwc_yarp device with a frameGrabber_nws_yarp device (streaming mode)")
    {
        do_nws_nwc_test(true);
    }

    Network::setLocalMode(false);
}
