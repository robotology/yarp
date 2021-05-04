/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Time.h>
#include <yarp/os/NetType.h>
#include <string>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("os::TimeTest", "[yarp::os]")
{
    SECTION("testing delay (there will be a short pause)...")
    {
        double target = 3.0;
        double t1 = Time::now();
        Time::delay(target);
        double t2 = Time::now();
        double dt = t2-t1-target;
        double limit = 2.0; // don't be too picky, there is a lot of undefined slop
        bool inLimits = (-limit<dt)&&(dt<limit);
        INFO(std::string("delay was late(+) or early(-) by ") +
               yarp::conf::numeric::to_string((int)(dt*1000)) +
               " ms");
        CHECK(inLimits); // delay for 3.0 seconds
    }
}
