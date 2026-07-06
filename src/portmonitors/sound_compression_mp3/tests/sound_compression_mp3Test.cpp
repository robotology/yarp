/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SystemClock.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundUtils.h>

#include <yarp/conf/environment.h>

#include <array>
#include <thread>
#include <cmath>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("pm::sound_compression_mp3Test", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("sound_compression_mp3", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    struct TestCase {
        std::string carrier;
        bool use_compression;
    };

    auto tc = GENERATE(
        TestCase {"fast_tcp", false},
        TestCase {"fast_tcp+send.portmonitor+file.sound_compression_mp3+type.dll+recv.portmonitor+file.sound_compression_mp3+type.dll", true}
    );

    SECTION("Test sound transmission with and without MP3 compression")
    {
        yarp::os::BufferedPort<yarp::sig::Sound> sender;
        yarp::os::BufferedPort<yarp::sig::Sound> receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        // Create a test sound with known properties
        const int frequency = 16000;
        const size_t samples = 1600;
        const size_t channels = 2;

        yarp::sig::Sound& snd = sender.prepare();
        snd.setFrequency(frequency);
        snd.resize(samples, channels);

        // Fill with a simple sine wave pattern
        yarp::sig::utils::makeTone(snd,0.1, channels, frequency, 440);

        sender.write();

        yarp::os::Time::delay(0.5);

        yarp::sig::Sound* received = receiver.read();
        REQUIRE(received != nullptr);

        // Verify basic properties are preserved
        CHECK(received->getFrequency() == frequency);
        CHECK(received->getSamples() == samples);
        CHECK(received->getChannels() == channels);

        // For MP3 compression, allow some tolerance due to lossy compression
        if (tc.use_compression) {
            // Just verify the sound has reasonable values (not all zeros)
            bool has_non_zero = false;
            for (size_t i = 0; i < samples && !has_non_zero; i++) {
                for (size_t c = 0; c < channels; c++) {
                    if (received->get(i, c) != 0) {
                        has_non_zero = true;
                        break;
                    }
                }
            }
            CHECK(has_non_zero);
        } else {
            // Without compression, data should match exactly
            for (size_t i = 0; i < samples; i++) {
                for (size_t c = 0; c < channels; c++) {
                    CHECK(received->get(i, c) == snd.get(i, c));
                }
            }
        }

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
