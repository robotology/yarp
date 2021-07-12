/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IRgbVisualParams.h>

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

        IRgbVisualParams* rgbParams = nullptr;
        REQUIRE(dd.view(rgbParams)); // interface rgb params reported

        // check the default parameters

        // checking fov
        double hfov=0.0;
        double vfov=0.0;
        rgbParams->getRgbFOV(hfov,vfov);
        CHECK(hfov == 1.0);
        CHECK(vfov == 2.0);

        // checking height
        CHECK(rgbParams->getRgbHeight() == 240);

        // checking width
        CHECK(rgbParams->getRgbWidth() == 320);

        // checking mirroring
        bool rgbMirroring;
        rgbParams->getRgbMirroring(rgbMirroring);
        CHECK_FALSE(rgbMirroring);

        // checking intrinsics
        rgbParams->getRgbIntrinsicParam(intrinsics);
        CHECK(intrinsics.find("focalLengthX").asFloat64() == 4.0); // checking focalLength X
        CHECK(intrinsics.find("focalLengthY").asFloat64() == 5.0); // checking focalLength Y
        CHECK(intrinsics.find("principalPointX").asFloat64() == 6.0); // checking principalPoint X
        CHECK(intrinsics.find("principalPointY").asFloat64() == 7.0); // checking principalPoint Y
        CHECK(intrinsics.find("k1").asFloat64() == 8.0); // checking k1
        CHECK(intrinsics.find("k2").asFloat64() == 9.0); // checking k2
        CHECK(intrinsics.find("k3").asFloat64() == 10.0); // checking k3
        CHECK(intrinsics.find("t1").asFloat64() == 11.0); // checking t1
        CHECK(intrinsics.find("t2").asFloat64() == 12.0);  // checking t2
        CHECK(intrinsics.find("distortionModel").asString() == "FishEye"); // checking distorionModel

        // checking the rectificationMatrix
        retM = intrinsics.find("rectificationMatrix").asList();
        double data[9]= {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
        Vector v(9,data);
        Vector v2;
        Portable::copyPortable(*retM,v2);
        for(int i=0; i<3; i++) {
            for(int j=0; j<3; j++) {
                CHECK(retM->get(i*3+j).asFloat64() == v(i*3+j));
                CHECK(v2(i*3+j) == v(i*3+j));
            }
        }

        // checking getRgbResolution
        int height, width;
        rgbParams->getRgbResolution(width,height);
        CHECK(width==320);
        CHECK(height==240);

        // checking configurations size
        VectorOf<CameraConfig> configurations;
        CHECK(rgbParams->getRgbSupportedConfigurations(configurations));
        CHECK(configurations.size() == 3);

        // checking first supported configuration
        CHECK(configurations[0].height == 128);
        CHECK(configurations[0].width == 128);
        CHECK(configurations[0].framerate == 60.0);
        CHECK(configurations[0].pixelCoding == VOCAB_PIXEL_RGB);

        // checking second supported configuration
        CHECK(configurations[1].height == 256);
        CHECK(configurations[1].width == 256);
        CHECK(configurations[1].framerate == 30.0);
        CHECK(configurations[1].pixelCoding == VOCAB_PIXEL_BGR);

        // checking third supported configuration
        CHECK(configurations[2].height == 512);
        CHECK(configurations[2].width == 512);
        CHECK(configurations[2].framerate == 15.0);
        CHECK(configurations[2].pixelCoding == VOCAB_PIXEL_MONO);

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
