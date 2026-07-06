/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SystemClock.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>

#include <yarp/conf/environment.h>

#include <array>
#include <thread>
#include <vector>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("pm::throttleDownTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("throttleDown", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(0.5);

    struct TestCase {
        std::string carrier;
        int period_ms;
        double send_rate;  // messages per second
        double expected_receive_rate;  // expected messages per second at receiver
        double tolerance;  // tolerance for rate comparison
    };

    auto tc = GENERATE(
        // Without throttle - should receive all messages
        TestCase {"fast_tcp", 0, 10.0, 10.0, 2.0},
        // With throttle - 500ms period = max 2 messages/second
        TestCase {"fast_tcp+send.portmonitor+type.dll+file.throttleDown+period_ms.500", 500, 10.0, 2.0, 1.0},
        // With throttle - 200ms period = max 5 messages/second
        TestCase {"fast_tcp+send.portmonitor+type.dll+file.throttleDown+period_ms.200", 200, 10.0, 5.0, 2.0}
    );

    SECTION("Test throttling behavior - sending at " + std::to_string(tc.send_rate) + " Hz, expecting ~" + std::to_string(tc.expected_receive_rate) + " Hz")
    {
        yarp::os::Port sender;
        yarp::os::Port receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        // Allow connection to establish
        yarp::os::Time::delay(0.5);

        // Send messages at specified rate for 2 seconds
        const double test_duration = 2.0;
        const double send_period = 1.0 / tc.send_rate;
        int messages_sent = 0;

        double start_time = yarp::os::Time::now();
        double next_send_time = start_time;

        while (yarp::os::Time::now() - start_time < test_duration) {
            if (yarp::os::Time::now() >= next_send_time) {
                yarp::os::Bottle& bottle = sender.prepare();
                bottle.clear();
                bottle.addInt32(messages_sent);
                sender.write();
                messages_sent++;
                next_send_time += send_period;
            }
            yarp::os::Time::delay(0.001);  // Small delay to avoid busy waiting
        }

        // Allow time for all messages to be processed
        yarp::os::Time::delay(0.5);

        // Count received messages
        int messages_received = 0;
        yarp::os::Bottle received;
        while (receiver.getPendingReads() > 0) {
            receiver.read(received);
            messages_received++;
        }

        double actual_duration = yarp::os::Time::now() - start_time;
        double actual_receive_rate = messages_received / actual_duration;

        yCInfo(THROTTLEDOWN_TEST) << "Sent" << messages_sent << "messages at rate" << tc.send_rate << "Hz";
        yCInfo(THROTTLEDOWN_TEST) << "Received" << messages_received << "messages at rate" << actual_receive_rate << "Hz";
        yCInfo(THROTTLEDOWN_TEST) << "Expected rate:" << tc.expected_receive_rate << "Hz (tolerance:" << tc.tolerance << "Hz)";

        // Verify the receive rate is within tolerance
        CHECK(actual_receive_rate >= tc.expected_receive_rate - tc.tolerance);
        CHECK(actual_receive_rate <= tc.expected_receive_rate + tc.tolerance);

        // Verify that throttling actually reduces message count when enabled
        if (tc.period_ms > 0) {
            CHECK(messages_received < messages_sent);
        }

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
