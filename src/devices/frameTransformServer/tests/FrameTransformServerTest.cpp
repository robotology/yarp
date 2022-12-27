/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <iostream>

#include <yarp/dev/tests/IFrameTransformTest.h>

#include <catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("dev::FrameTransformServerTest", "[yarp::dev]")
{
    #if defined(DISABLE_FAILING_TESTS)
        YARP_SKIP_TEST("Skipping failing tests")
    #endif

    YARP_REQUIRE_PLUGIN("frameTransformServer", "device");

    yarp::os::Network::setLocalMode(true);
    const bool verboseDebug = true;

    SECTION("test the frameTransformServer open/close")
    {
        yarp::dev::PolyDriver pd;
        yarp::os::Property p;

        p.put("device", "frameTransformServer");
        p.put("filexml_option", "fts_yarp_only.xml");
        REQUIRE(pd.open(p));
        yarp::os::Time::delay(0.5);
        REQUIRE(pd.close());
    }

    yarp::os::Network::setLocalMode(false);
}
