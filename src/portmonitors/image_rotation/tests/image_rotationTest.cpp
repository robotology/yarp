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

TEST_CASE("pm::image_rotationTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("image_rotation", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    struct TestCase {
        std::string carrier;
        size_t expected_width;
        size_t expected_height;
    };

    auto tc = GENERATE(
        // No rotation - dimensions stay same
        TestCase {"fast_tcp", 20, 10},
        // 90 degree clockwise rotation - dimensions swap
        TestCase {"fast_tcp+recv.portmonitor+file.image_rotation+options_rotate.rotate_cw+type.dll", 10, 20},
        // 90 degree counter-clockwise rotation - dimensions swap
        TestCase {"fast_tcp+recv.portmonitor+file.image_rotation+options_rotate.rotate_ccw+type.dll", 10, 20},
        // 180 degree rotation - dimensions stay same
        TestCase {"fast_tcp+recv.portmonitor+file.image_rotation+options_rotate.rotate_180+type.dll", 20, 10}
    );

    SECTION("Test image rotation with expected dimensions: " + std::to_string(tc.expected_width) + "x" + std::to_string(tc.expected_height))
    {
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>> sender;
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>> receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        // Create a test image with known dimensions (wider than tall)
        const size_t original_width = 20;
        const size_t original_height = 10;

        yarp::sig::ImageOf<yarp::sig::PixelRgb>& img = sender.prepare();
        img.resize(original_width, original_height);

        // Fill with a pattern to verify rotation
        for (size_t y = 0; y < original_height; y++) {
            for (size_t x = 0; x < original_width; x++) {
                img.pixel(x, y).r = static_cast<unsigned char>(x * 10);
                img.pixel(x, y).g = static_cast<unsigned char>(y * 20);
                img.pixel(x, y).b = 128;
            }
        }

        sender.write();

        yarp::os::Time::delay(0.5);

        yarp::sig::ImageOf<yarp::sig::PixelRgb>* received = receiver.read();
        REQUIRE(received != nullptr);

        // Verify dimensions changed according to rotation
        CHECK(received->width() == tc.expected_width);
        CHECK(received->height() == tc.expected_height);

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
