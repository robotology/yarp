/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Time.h>
#include <yarp/os/NetType.h>
#include <string>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;

#include <chrono>
#include <thread>
void test_os_delay(double target, double limit, bool strict_test)
{
    std::chrono::system_clock::time_point t1a = std::chrono::system_clock::now();
    double t1 = std::chrono::duration_cast<std::chrono::microseconds>(t1a.time_since_epoch()).count()/1e6;

    std::chrono::duration<double> duration_seconds(target);
    std::this_thread::sleep_for(duration_seconds);

    std::chrono::system_clock::time_point t2a = std::chrono::system_clock::now();
    double t2 = std::chrono::duration_cast<std::chrono::microseconds>(t2a.time_since_epoch()).count()/1e6;

    double dt = t2 - t1 - target;
    bool inLimits = (-limit < dt) && (dt < limit);
    std::string s = std::string("OS: tested delay of ") + std::to_string(target) +
        std::string("s, delay was late(+) or early(-) by ") +
        std::to_string((double)(dt * 1000)) +
        std::string(" ms");
    if (inLimits) {
        s += "-> OK!";
    }
    else {
        s += "-> ERR!";
    }
    INFO(s);
    printf("%s\n", s.c_str());

    //if strict_test == true, the test will generate a real error,
    //otherwise it will just print some messages, without generating an error.
    //Beware: on some machines, os, cloud/virtual environments, this test might fail
    //for small values of limit. Hence it is not suitable to run on a cloud CI,
    //(eventually it should be marked with `DISABLE_FAILING_TESTS`)
    if (strict_test) { CHECK(inLimits); }
}

void test_yarp_delay(double target, double limit, bool strict_test)
{
    double t1 = Time::now();
    Time::delay(target);
    double t2 = Time::now();
    double dt = t2 - t1 - target;
    bool inLimits = (-limit < dt) && (dt < limit);
    std::string s = std::string("YARP: tested delay of ") + std::to_string(target) +
        std::string("s, delay was late(+) or early(-) by ") +
        std::to_string((double)(dt * 1000)) +
        std::string(" ms");
    if (inLimits)  {
        s += "-> OK!"; }
    else    {
        s += "-> ERR!"; }
    INFO(s);
    printf("%s\n", s.c_str());

    //if strict_test == true, the test will generate a real error,
    //otherwise it will just print some messages, without generating an error.
    //Beware: on some machines, os, cloud/virtual environments, this test might fail
    //for small values of limit. Hence it is not suitable to run on a cloud CI,
    //(eventually it should be marked with `DISABLE_FAILING_TESTS`)
    if (strict_test) { CHECK(inLimits); }
}

TEST_CASE("os::TimeTest", "[yarp::os]")
{
    SECTION("testing delay using YARP (there will be a short pause)...")
    {
        //the original test (till yarp 3.6)
        test_yarp_delay ( 3.000, 2.000, true);

        //additional tests which do not generate an error but just prints
        test_yarp_delay ( 3.000, 0.010, false);
        test_yarp_delay ( 0.300, 0.010, false);
        test_yarp_delay ( 0.030, 0.010, false);
    }

    SECTION("testing delay using C++ (there will be a short pause)...")
    {
        test_os_delay(3.000, 0.010, true);
        test_os_delay(0.300, 0.010, true);
        test_os_delay(0.030, 0.010, true);
    }
}
