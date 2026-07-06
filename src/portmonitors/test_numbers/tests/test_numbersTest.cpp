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
    YARP_REQUIRE_PLUGIN("test_numbers", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    struct TestCase {
        std::string carrier;
        int expected_value;
    };

    auto tc = GENERATE(
        TestCase {"fast_tcp", 1},
        //send
        TestCase {"fast_tcp+send.portmonitor+type.dll+file.test_numbers1+type.dll",2}, //1+1
        TestCase {"fast_tcp+send.portmonitor+type.dll+file.test_numbers1+file1.test_numbers1+type1.dll", 3}, //1+1+1
        TestCase {"fast_tcp+send.portmonitor+type.dll+file.test_numbers1+file1.test_numbers2+type1.dll", 4}, //(1+1)*2
        TestCase {"fast_tcp+send.portmonitor+type.dll+file.test_numbers2+file1.test_numbers1+type1.dll", 3}, //(1*2)+1
        //recv
        TestCase {"fast_tcp+recv.portmonitor+type.dll+file.test_numbers1+type.dll",2}, //1+1
        TestCase {"fast_tcp+recv.portmonitor+type.dll+file.test_numbers1+file1.test_numbers1+type1.dll", 3}, //1+1+1
        TestCase {"fast_tcp+recv.portmonitor+type.dll+file.test_numbers1+file1.test_numbers2+type1.dll", 4}, //(1+1)*2
        TestCase {"fast_tcp+recv.portmonitor+type.dll+file.test_numbers2+file1.test_numbers1+type1.dll", 3}, //(1*2)+1
        //mixed
        TestCase {"fast_tcp+send.portmonitor+type.dll+file.test_numbers1+type.dll+recv.portmonitor+type.dll+file.test_numbers1+type.dll",3}, //1+1+1
        TestCase {"fast_tcp+send.portmonitor+type.dll+file.test_numbers1+file1.test_numbers1+type1.dll+recv.portmonitor+type.dll+file.test_numbers1+file1.test_numbers1+type1.dll", 5}, //1+1+1+1+1
        TestCase {"fast_tcp+send.portmonitor+type.dll+file.test_numbers1+file1.test_numbers2+type1.dll+recv.portmonitor+type.dll+file.test_numbers1+file1.test_numbers2+type1.dll", 10}, //((1+1)*2+1)*2
        TestCase {"fast_tcp+send.portmonitor+type.dll+file.test_numbers2+file1.test_numbers1+type1.dll+recv.portmonitor+type.dll+file.test_numbers2+file1.test_numbers1+type1.dll", 7}  //((1*2)+1)*2+1
    );

    SECTION("Test that normally the portmonitor is not used")
    {
        yarp::os::Port sender;
        yarp::os::Port receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        sender.write(yarp::os::Value(tc.expected_value));

        yarp::os::Time::delay(0.5);

        yarp::os::Value rval;
        receiver.read(rval);
        CHECK(rval.asInt64()==tc.expected_value);

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
