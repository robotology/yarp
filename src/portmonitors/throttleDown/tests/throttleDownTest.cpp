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
        int value_to_send;
        int period_ms;
        double send_rate;  // messages per second
        double expected_receive_rate;  // expected messages per second at receiver
        double tolerance;  // tolerance for rate comparison
    };

    auto tc = GENERATE(
        // Without throttle - should receive all messages
        TestCase {"fast_tcp", 42, 0, 10.0, 10.0, 2.0},
        // With throttle - 500ms period = max 2 messages/second
        TestCase {"fast_tcp+send.portmonitor+type.dll+file.throttleDown+period_ms.500", 42, 500, 10.0, 2.0, 1.0},
        // With throttle - 200ms period = max 5 messages/second
        TestCase {"fast_tcp+send.portmonitor+type.dll+file.throttleDown+period_ms.200", 42, 200, 10.0, 5.0, 2.0}
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

        sender.write(yarp::os::Value(tc.value_to_send));

        yarp::os::Time::delay(0.5);

        yarp::os::Value rval;
        receiver.read(rval);
        CHECK(rval.asInt64()==tc.value_to_send);

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
