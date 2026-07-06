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

#include <yarp/conf/environment.h>

#include <array>
#include <thread>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("pm::sound_markerTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("sound_marker", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    struct TestCase {
        std::string carrier;
        bool use_marker;
    };

    auto tc = GENERATE(
        TestCase {"fast_tcp", false},
        TestCase {"fast_tcp+recv.portmonitor+file.sound_marker+type.dll", true}
    );

    SECTION("Test sound transmission with and without marker")
    {
        yarp::os::BufferedPort<yarp::sig::Sound> sender;
        yarp::os::BufferedPort<yarp::sig::Sound> receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        // Create a test sound
        const int frequency = 16000;
        const size_t samples = 1600;
        const size_t channels = 1;

        yarp::sig::Sound& snd = sender.prepare();
        snd.setFrequency(frequency);
        snd.resize(samples, channels);

        // Fill with simple data
        for (size_t i = 0; i < samples; i++) {
            snd.set(static_cast<yarp::sig::Sound::audio_sample>(i % 1000), i, 0);
        }

        sender.write();

        yarp::os::Time::delay(0.5);

        yarp::sig::Sound* received = receiver.read();
        REQUIRE(received != nullptr);

        // Verify basic properties
        CHECK(received->getFrequency() == frequency);
        CHECK(received->getChannels() == channels);

        if (tc.use_marker) {
            // When marker is used, the sound should have markers added
            // The size might be different due to markers at beginning/end
            CHECK(received->getSamples() >= samples);
        } else {
            // Without marker, size should match exactly
            CHECK(received->getSamples() == samples);
        }

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
