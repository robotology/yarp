/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/LogStream.h>

#if defined(USE_SYSTEM_CATCH)
#include <catch.hpp>
#else
#include "catch.hpp"
#endif

TEST_CASE("OS::LogStreamTest", "[yarp::os]") {

    int i = 13;
    std::vector<int> v(4);
    v[0] = 1;
    v[1] = 2;
    v[2] = 3;
    v[3] = 4;

    SECTION("Test yTrace") {
        yTrace("This is a trace");
        yTrace("This is %s (%d)", "a trace", i);
        yTrace();
        yTrace() << "This is" << "another" << "trace" << i;
        yTrace() << v;
    }

    SECTION("Test yDebug") {
        yDebug("This is a debug");
        yDebug("This is %s (%d)", "a debug", i);
        yDebug();
        yDebug() << "This is" << "another" << "debug" << i;
        yDebug() << v;
    }

    SECTION("Test yInfo") {
        yInfo("This is info");
        yInfo("This is %s (%d)", "info", i);
        yInfo();
        yInfo() << "This is" << "more" << "info" << i;
        yInfo() << v;
    }

    SECTION("Test yWarning") {
        yWarning("This is a warning");
        yWarning("This is %s (%d)", "a warning", i);
        yWarning();
        yWarning() << "This is" << "another" << "warning" << i;
        yWarning() << v;
    }
    SECTION("Test yError") {
        yError("This is an error");
        yError("This is %s (%d)", "an error", i);
        yError();
        yError() << "This is" << "another" << "error" << i;
        yError() << v;
    }

}
