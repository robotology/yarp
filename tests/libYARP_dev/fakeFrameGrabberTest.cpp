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

#include <array>
#include <string>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


TEST_CASE("dev::fakeFrameGrabberTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeFrameGrabber", "device");

    Network::setLocalMode(true);

    constexpr int default_height = 240;
    constexpr int default_width = 320;
    constexpr double default_hfov = 1.0;
    constexpr double default_vfov = 2.0;
    constexpr double default_physFocalLength = 3.0;
    constexpr double default_focalLengthX = 4.0;
    constexpr double default_focalLengthY = 5.0;
    constexpr double default_principalPointX = 6.0;
    constexpr double default_principalPointY = 7.0;
    constexpr double default_k1 = 8.0;
    constexpr double default_k2 = 9.0;
    constexpr double default_k3 = 10.0;
    constexpr double default_t1 = 11.0;
    constexpr double default_t2 = 12.0;
    const std::string default_distortionModel = "FishEye";
    constexpr std::array<double, 9> default_rectificationMatrix = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    const std::vector<yarp::dev::CameraConfig> default_configurations = {
        { 128, 128, 60.0, VOCAB_PIXEL_RGB },
        { 256, 256, 30.0, VOCAB_PIXEL_BGR },
        { 512, 512, 15.0, VOCAB_PIXEL_MONO }
    };

    constexpr size_t crop_h = 24;
    constexpr size_t crop_w = 32;

    const yarp::sig::VectorOf<std::pair<int, int>> vertices {{0, 0}, {crop_w - 1, crop_h - 1}};

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

        // Check width()
        CHECK(iFrameGrabberImage->width() == default_width);

        // Check height()
        CHECK(iFrameGrabberImage->height() == default_height);

        // Check getImage()
        ImageOf<PixelRgb> img;
        iFrameGrabberImage->getImage(img);
        CHECK(img.width() == default_width);
        CHECK(img.height() == default_height);

        // Check getImageCrop()
        ImageOf<PixelRgb> crop;
        CHECK(iFrameGrabberImage->getImageCrop(YARP_CROP_RECT, vertices, crop));
        CHECK(crop.width() == crop_w);
        CHECK(crop.height() == crop_h);

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

        // Check getRgbFOV()
        double hfov;
        double vfov;
        rgbParams->getRgbFOV(hfov, vfov);
        CHECK(hfov == default_hfov);
        CHECK(vfov == default_vfov);

        // Check getRgbHeight() and getRgbWidth()
        CHECK(rgbParams->getRgbWidth() == default_width);
        CHECK(rgbParams->getRgbHeight() == default_height);

        // Check getRgbMirroring()
        bool rgbMirroring;
        rgbParams->getRgbMirroring(rgbMirroring);
        CHECK_FALSE(rgbMirroring);

        // Check getRgbIntrinsicParam()
        Property intrinsics;
        rgbParams->getRgbIntrinsicParam(intrinsics);
        CHECK(intrinsics.find("physFocalLength").asFloat64() == default_physFocalLength);
        CHECK(intrinsics.find("focalLengthX").asFloat64() == default_focalLengthX);
        CHECK(intrinsics.find("focalLengthY").asFloat64() == default_focalLengthY);
        CHECK(intrinsics.find("principalPointX").asFloat64() == default_principalPointX);
        CHECK(intrinsics.find("principalPointY").asFloat64() == default_principalPointY);
        CHECK(intrinsics.find("k1").asFloat64() == default_k1);
        CHECK(intrinsics.find("k2").asFloat64() == default_k2);
        CHECK(intrinsics.find("k3").asFloat64() == default_k3);
        CHECK(intrinsics.find("t1").asFloat64() == default_t1);
        CHECK(intrinsics.find("t2").asFloat64() == default_t2);
        CHECK(intrinsics.find("distortionModel").asString() == default_distortionModel);
        Bottle* retMat = intrinsics.find("rectificationMatrix").asList();
        for(int i=0; i<3; i++) {
            for(int j=0; j<3; j++) {
                CHECK(retMat->get(i*3+j).asFloat64() == default_rectificationMatrix[i*3+j]);
            }
        }

        // Checking getRgbResolution()
        int height;
        int width;
        rgbParams->getRgbResolution(width,height);
        CHECK(width == default_width);
        CHECK(height == default_height);

        // Check getRgbSupportedConfigurations()
        VectorOf<CameraConfig> configurations;
        CHECK(rgbParams->getRgbSupportedConfigurations(configurations));
        CHECK(configurations.size() == 3);
        // checking first supported configuration
        CHECK(configurations[0].height == default_configurations[0].height);
        CHECK(configurations[0].width == default_configurations[0].width);
        CHECK(configurations[0].framerate == default_configurations[0].framerate);
        CHECK(configurations[0].pixelCoding == default_configurations[0].pixelCoding);
        // checking second supported configuration
        CHECK(configurations[1].height == default_configurations[1].height);
        CHECK(configurations[1].width == default_configurations[1].width);
        CHECK(configurations[1].framerate == default_configurations[1].framerate);
        CHECK(configurations[1].pixelCoding == default_configurations[1].pixelCoding);
        // checking third supported configuration
        CHECK(configurations[2].height == default_configurations[2].height);
        CHECK(configurations[2].width == default_configurations[2].width);
        CHECK(configurations[2].framerate == default_configurations[2].framerate);
        CHECK(configurations[2].pixelCoding == default_configurations[2].pixelCoding);

        // Close the device
        CHECK(dd.close()); // client close reported successful
    }

    // TODO Add tests for the other interfaces

    Network::setLocalMode(false);
}
