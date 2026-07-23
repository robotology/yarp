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
#include <cmath>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("pm::depthimage_to_rgbTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("depthimage_to_rgb", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    struct TestCase {
        std::string carrier;
        bool should_convert;
    };

    auto tc = GENERATE(
        TestCase {"fast_tcp", false},
        TestCase {"fast_tcp+recv.portmonitor+file.depthimage_to_rgb+type.dll", true}
    );

    SECTION("Test depth image to RGB heatmap conversion with portmonitor: " + tc.carrier)
    {
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelFloat>> sender;
        yarp::os::BufferedPort<yarp::sig::FlexImage> receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        // Create a test depth image (16x16 pixels)
        constexpr int width = 16;
        constexpr int height = 16;

        yarp::sig::ImageOf<yarp::sig::PixelFloat>& depthImg = sender.prepare();
        depthImg.resize(width, height);

        // Fill with test data - values between 0.2 and 10.0 meters
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                float depth_value = 0.2f + (static_cast<float>(x + y * width) / (width * height - 1)) * 9.8f;
                depthImg.pixel(x, y) = depth_value;
            }
        }

        sender.write();

        yarp::os::Time::delay(0.5);

        yarp::sig::FlexImage* receivedImg = receiver.read();
        REQUIRE(receivedImg != nullptr);

        // Verify dimensions
        CHECK(receivedImg->width() == width);
        CHECK(receivedImg->height() == height);

        if (tc.should_convert)
        {
            // Verify it's converted to RGB heatmap
            CHECK(receivedImg->getPixelCode() == VOCAB_PIXEL_RGB);
            CHECK(receivedImg->getPixelSize() == 3);

            // Verify some pixels have valid RGB values
            unsigned char* pixels = receivedImg->getRawImage();
            bool hasNonZeroPixel = false;
            for (int i = 0; i < width * height * 3; i++)
            {
                CHECK(pixels[i] <= 255);
                if (pixels[i] > 0)
                {
                    hasNonZeroPixel = true;
                }
            }
            // At least some pixels should have color (not all black)
            CHECK(hasNonZeroPixel);
        }
        else
        {
            // Without conversion, should still be float
            CHECK(receivedImg->getPixelCode() == VOCAB_PIXEL_MONO_FLOAT);
        }

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
