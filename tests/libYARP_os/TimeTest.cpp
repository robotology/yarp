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

void test_delay(double target, double limit, bool strict_test)
{
    double t1 = Time::now();
    Time::delay(target);
    double t2 = Time::now();
    double dt = t2 - t1 - target;
    bool inLimits = (-limit < dt) && (dt < limit);
    std::string s = std::string("tested delay of ") + std::to_string(target) +
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
    SECTION("testing delay (there will be a short pause)...")
    {
        //the original test (till yarp 3.6)
        test_delay ( 3.000, 2.000, true);

        //additional tests which do not generate an error but just prints
        test_delay ( 3.000, 0.010, false);
        test_delay ( 0.300, 0.010, false);
        test_delay ( 0.030, 0.010, false);
    }
}
