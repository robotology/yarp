/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Image.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IWrapper.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


TEST_CASE("dev::frameGrabberCropperTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeFrameGrabber", "device");
    YARP_REQUIRE_PLUGIN("frameGrabberCropper", "device");

    Network::setLocalMode(true);

    SECTION("test the frameGrabberCropper mode")
    {
        constexpr size_t width = 320;
        constexpr size_t height = 240;

        // Open fakeFrameGrabber
        PolyDriver dd;
        Property p;
        p.put("device", "fakeFrameGrabber");
        p.put("width", static_cast<int>(width * 2));
        p.put("height", static_cast<int>(height));
        REQUIRE(dd.open(p));

        // Open frameGrabberCropper left
        PolyDriver ddl;
        Property pl;
        pl.put("device","frameGrabberCropper");
        pl.put("x1", 0);
        pl.put("y1", 0);
        pl.put("x2", static_cast<int>(width - 1));
        pl.put("y2", static_cast<int>(height - 1));
        REQUIRE(ddl.open(pl));

        // Attach frameGrabberCropper left
        yarp::dev::IWrapper* wl = nullptr;
        REQUIRE(ddl.view(wl));
        REQUIRE(wl->attach(&dd));

        // Check IFrameGrabberImage on frameGrabberCropper left
        IFrameGrabberImage *ifgil = nullptr;
        REQUIRE(ddl.view(ifgil));

        ImageOf<PixelRgb> imgl;
        ifgil->getImage(imgl);
        CHECK(imgl.width() == width);
        CHECK(imgl.height() == height);


        // Open frameGrabberCropper right
        PolyDriver ddr;
        Property pr;
        pr.put("device","frameGrabberCropper");
        pr.put("x1", static_cast<int>(width));
        pr.put("y1", static_cast<int>(0));
        pr.put("x2", static_cast<int>((width * 2) - 1));
        pr.put("y2", static_cast<int>(height - 1));
        REQUIRE(ddr.open(pr));

        // Attach frameGrabberCropper right
        yarp::dev::IWrapper* wr = nullptr;
        REQUIRE(ddr.view(wr));
        REQUIRE(wr->attach(&dd));

        // Check IFrameGrabberImage on frameGrabberCropper right
        IFrameGrabberImage *ifgir = nullptr;
        REQUIRE(ddr.view(ifgir));

        ImageOf<PixelRgb> imgr;
        ifgir->getImage(imgr);
        CHECK(imgr.width() == width);
        CHECK(imgr.height() == height);


        // Close all devices
        CHECK(ddl.close());
        CHECK(ddr.close());
        CHECK(dd.close());
    }

    Network::setLocalMode(false);
}
