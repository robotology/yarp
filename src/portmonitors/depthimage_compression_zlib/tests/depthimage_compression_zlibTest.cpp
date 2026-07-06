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

TEST_CASE("pm::depthimage_compression_zlibTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("depthimage_compression_zlib", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    struct TestCase {
        std::string carrier;
        bool should_compress;
    };

    auto tc = GENERATE(
        TestCase {"fast_tcp", false},
        TestCase {"fast_tcp+send.portmonitor+file.depthimage_compression_zlib+type.dll+recv.portmonitor+file.depthimage_compression_zlib+type.dll", true}
    );

    SECTION("Test depth image compression with portmonitor: " + tc.carrier)
    {
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelFloat>> sender;
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelFloat>> receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        // Create a test depth image (10x10 pixels)
        constexpr int width = 10;
        constexpr int height = 10;

        yarp::sig::ImageOf<yarp::sig::PixelFloat>& img = sender.prepare();
        img.resize(width, height);

        // Fill with test data - create a gradient pattern
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                float depth_value = static_cast<float>(x + y * width) / 10.0f;
                img.pixel(x, y) = depth_value;
            }
        }

        sender.write();

        yarp::os::Time::delay(0.5);

        yarp::sig::ImageOf<yarp::sig::PixelFloat>* receivedImg = receiver.read();
        REQUIRE(receivedImg != nullptr);

        // Verify dimensions
        CHECK(receivedImg->width() == width);
        CHECK(receivedImg->height() == height);

        // Verify pixel data matches
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                float expected_value = static_cast<float>(x + y * width) / 10.0f;
                float actual_value = receivedImg->pixel(x, y);
                CHECK(actual_value == expected_value);
            }
        }

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
