/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SystemClock.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>

#include <yarp/conf/environment.h>

#include <array>
#include <thread>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("pm::stats_monitorTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("simulated_network_delay", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    auto tc = GENERATE(
        "fast_tcp",
        "fast_tcp+send.portmonitor+file.simulated_network_delay+delay_ms.10+type.dll"
    );

    SECTION("Test that normally the portmonitor is not used")
    {
        yarp::os::Port sender;
        yarp::os::Port receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc);
        REQUIRE(b);

        yarp::os::Value val(10);
        sender.write(val);

        yarp::os::Time::delay(0.5);

        yarp::os::Value recv;

        receiver.read(recv);
        CHECK(recv.asInt16()== 10);

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
