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

TEST_CASE("pm::soundfilter_resampleTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("soundfilter_resample", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    struct TestCase {
        std::string carrier;
        int expected_frequency;
    };

    auto tc = GENERATE(
        TestCase {"fast_tcp", 16000},
        TestCase {"fast_tcp+recv.portmonitor+file.soundfilter_resample+recv.target_sample_rate+8000+type.dll", 8000}
    );

    SECTION("Test sound resampling")
    {
        yarp::os::BufferedPort<yarp::sig::Sound> sender;
        yarp::os::BufferedPort<yarp::sig::Sound> receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        // Create a test sound with 16000 Hz
        const int original_frequency = 16000;
        const size_t samples = 1600;
        const size_t channels = 2;

        yarp::sig::Sound& snd = sender.prepare();
        snd.setFrequency(original_frequency);
        snd.resize(samples, channels);

        // Fill with test data
        for (size_t i = 0; i < samples; i++) {
            for (size_t c = 0; c < channels; c++) {
                snd.set(static_cast<yarp::sig::Sound::audio_sample>(i), i, c);
            }
        }

        sender.write();

        yarp::os::Time::delay(0.5);

        yarp::sig::Sound* received = receiver.read();
        REQUIRE(received != nullptr);

        // Verify the frequency is as expected (original or resampled)
        CHECK(received->getFrequency() == tc.expected_frequency);
        CHECK(received->getChannels() == channels);

        // If resampled, the number of samples should be proportional to the frequency ratio
        if (tc.expected_frequency != original_frequency) {
            size_t expected_samples = samples * tc.expected_frequency / original_frequency;
            // Allow some tolerance in the resampled size
            CHECK(received->getSamples() >= expected_samples - 10);
            CHECK(received->getSamples() <= expected_samples + 10);
        } else {
            CHECK(received->getSamples() == samples);
        }

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
