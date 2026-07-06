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

TEST_CASE("pm::depthimage_compression_zfpTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("depthimage_compression_zfp", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    struct TestCase {
        std::string carrier;
        bool use_compression;
    };

    auto tc = GENERATE(
        TestCase {"fast_tcp", false},
        TestCase {"fast_tcp+send.portmonitor+file.depthimage_compression_zfp+recv.portmonitor+file.depthimage_compression_zfp+type.dll", true}
    );

    SECTION("Test depth image ZFP compression")
    {
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelMono>> sender;
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelMono>> receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        // Create a test depth image
        const size_t width = 64;
        const size_t height = 48;

        yarp::sig::ImageOf<yarp::sig::PixelMono>& img = sender.prepare();
        img.resize(width, height);

        // Fill with gradient pattern
        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                img.pixel(x, y) = static_cast<unsigned char>((x + y) % 256);
            }
        }

        sender.write();

        yarp::os::Time::delay(0.5);

        yarp::sig::ImageOf<yarp::sig::PixelMono>* received = receiver.read();
        REQUIRE(received != nullptr);

        // Verify dimensions are preserved
        CHECK(received->width() == width);
        CHECK(received->height() == height);

        if (tc.use_compression) {
            // With lossy compression, verify data is approximately correct
            bool has_similar_values = true;
            for (size_t y = 0; y < height && has_similar_values; y++) {
                for (size_t x = 0; x < width; x++) {
                    unsigned char original = img.pixel(x, y);
                    unsigned char compressed = received->pixel(x, y);
                    // Allow some tolerance for lossy compression
                    if (std::abs(static_cast<int>(original) - static_cast<int>(compressed)) > 50) {
                        has_similar_values = false;
                        break;
                    }
                }
            }
            CHECK(has_similar_values);
        } else {
            // Without compression, should match exactly
            for (size_t y = 0; y < height; y++) {
                for (size_t x = 0; x < width; x++) {
                    CHECK(received->pixel(x, y) == img.pixel(x, y));
                }
            }
        }

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
