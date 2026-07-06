/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SystemClock.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Image.h>

#include <yarp/conf/environment.h>

#include <array>
#include <thread>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("pm::segmentationimage_to_rgbTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("segmentationimage_to_rgb", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    struct TestCase {
        std::string carrier;
        bool use_converter;
    };

    auto tc = GENERATE(
        TestCase {"fast_tcp", false},
        TestCase {"fast_tcp+recv.portmonitor+file.segmentationimage_to_rgb+type.dll", true}
    );

    SECTION("Test segmentation image to RGB conversion")
    {
        yarp::os::BufferedPort<yarp::sig::FlexImage> sender;
        yarp::os::BufferedPort<yarp::sig::FlexImage> receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        // Create a test segmentation image (mono image with pixel codes)
        const size_t width = 10;
        const size_t height = 10;

        yarp::sig::FlexImage& img = sender.prepare();
        img.setPixelCode(VOCAB_PIXEL_MONO);
        img.resize(width, height);

        // Fill with some segmentation labels (0-255)
        unsigned char* rawImg = img.getRawImage();
        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                unsigned char label = static_cast<unsigned char>((x + y) % 10);
                rawImg[y * width + x] = label;
            }
        }

        sender.write();

        yarp::os::Time::delay(0.5);

        yarp::sig::FlexImage* received = receiver.read();
        REQUIRE(received != nullptr);

        CHECK(received->width() == width);
        CHECK(received->height() == height);

        if (tc.use_converter) {
            // Should be converted to RGB
            CHECK(received->getPixelCode() == VOCAB_PIXEL_RGB);
        } else {
            // Should remain mono
            CHECK(received->getPixelCode() == VOCAB_PIXEL_MONO);
        }

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
