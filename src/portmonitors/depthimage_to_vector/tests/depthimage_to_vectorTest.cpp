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
#include <yarp/sig/Vector.h>

#include <yarp/conf/environment.h>

#include <array>
#include <thread>
#include <cmath>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("pm::depthimage_to_vectorTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("depthimage_to_vector", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    struct TestCase {
        std::string carrier;
        bool should_convert;
    };

    auto tc = GENERATE(
        TestCase {"fast_tcp", false},
        TestCase {"fast_tcp+recv.portmonitor+file.depthimage_to_vector+type.dll", true}
    );

    SECTION("Test depth image to vector conversion with portmonitor: " + tc.carrier)
    {
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelFloat>> sender;
        yarp::os::Port receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        // Create a test depth image (5x4 pixels = 20 total values for easy verification)
        constexpr int width = 5;
        constexpr int height = 4;
        constexpr int total_pixels = width * height;

        yarp::sig::ImageOf<yarp::sig::PixelFloat>& depthImg = sender.prepare();
        depthImg.resize(width, height);

        // Fill with test data - sequential values for easy verification
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                float depth_value = static_cast<float>(x + y * width) + 0.5f;
                depthImg.pixel(x, y) = depth_value;
            }
        }

        sender.write();

        yarp::os::Time::delay(0.5);

        if (tc.should_convert)
        {
            // When converted, expect a Vector
            yarp::sig::Vector receivedVec;
            receiver.read(receivedVec);

            // Verify size
            CHECK(receivedVec.size() == total_pixels);

            // Verify the values match the input
            for (int i = 0; i < total_pixels; i++)
            {
                float expected_value = static_cast<float>(i) + 0.5f;
                CHECK(receivedVec[i] == expected_value);
            }
        }
        else
        {
            // Without conversion, should still be an image
            yarp::sig::ImageOf<yarp::sig::PixelFloat> receivedImg;
            receiver.read(receivedImg);

            CHECK(receivedImg.width() == width);
            CHECK(receivedImg.height() == height);
        }

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
