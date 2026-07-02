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
    YARP_REQUIRE_PLUGIN("stats_monitor", "portmonitor")
    YARP_REQUIRE_PLUGIN("throttleDown", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);
    bool b;
    std::string s1 =  "/stats_monitor/send/recv/stats:o";
    std::string s2 =  "/stats_monitor/recv/send/stats:o";

    SECTION("Test that normally the portmonitor is not used")
    {
        yarp::os::Port sender;
        yarp::os::Port receiver;

        sender.open("/send");
        receiver.open("/recv");

        yarp::os::Network::connect("/send", "/recv");

        yarp::os::Time::delay(1.0);
        b = yarp::os::Network::exists(s1);
        REQUIRE(!b);
        b = yarp::os::Network::exists(s2);
        REQUIRE(!b);

        receiver.close();
        sender.close();

        yarp::os::Time::delay(1.0);
        b = yarp::os::Network::exists(s1);
        REQUIRE(!b);
        b = yarp::os::Network::exists(s2);
        REQUIRE(!b);
    }

    SECTION("Test standard usage of stats_monitor")
    {
        yarp::os::Port sender;
        yarp::os::Port receiver;

        sender.open("/send");
        receiver.open("/recv");

        yarp::os::Network::connect("/send", "/recv","tcp+send.portmonitor+file.stats_monitor+type.dll");

        yarp::os::Time::delay(1.0);
        b = yarp::os::Network::exists(s1);
        REQUIRE(b);
        b = yarp::os::Network::exists(s2);
        REQUIRE(b);

        receiver.close();
        sender.close();

        yarp::os::Time::delay(1.0);
        b = yarp::os::Network::exists(s1);
        REQUIRE(!b);
        b = yarp::os::Network::exists(s2);
        REQUIRE(!b);
    }

    SECTION("Test usage od environment variable ENABLE_CONNECTIONS_STATS")
    {
        yarp::os::Port sender;
        yarp::os::Port receiver;

        yarp::conf::environment::setEnvironment("ENABLE_CONNECTIONS_STATS", "1" );

        sender.open("/send");
        receiver.open("/recv");

        yarp::os::Network::connect("/send", "/recv");

        yarp::os::Time::delay(1.0);
        b = yarp::os::Network::exists(s1);
        REQUIRE(b);
        b = yarp::os::Network::exists(s2);
        REQUIRE(b);

        receiver.close();
        sender.close();

        yarp::os::Time::delay(1.0);
        b = yarp::os::Network::exists(s1);
        REQUIRE(!b);
        b = yarp::os::Network::exists(s2);
        REQUIRE(!b);

        yarp::conf::environment::setEnvironment("YARP_CONNECTIONS_STATS_ENABLE", "" );
    }

    SECTION("Test multiple monitors concatenation")
    {
        yarp::os::Port sender;
        yarp::os::Port receiver;

        yarp::conf::environment::setEnvironment("YARP_CONNECTIONS_STATS_ENABLE", "1" );

        sender.open("/send");
        receiver.open("/recv");

        yarp::os::Network::connect("/send", "/recv", "tcp+send.portmonitor+type.dll+file.throttleDown+period_ms.500");

        yarp::os::Time::delay(1.0);
        b = yarp::os::Network::exists(s1);
        REQUIRE(b);
        b = yarp::os::Network::exists(s2);
        REQUIRE(b);

        receiver.close();
        sender.close();

        yarp::os::Time::delay(1.0);
        b = yarp::os::Network::exists(s1);
        REQUIRE(!b);
        b = yarp::os::Network::exists(s2);
        REQUIRE(!b);

        yarp::conf::environment::setEnvironment("YARP_CONNECTIONS_STATS_ENABLE", "" );
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
