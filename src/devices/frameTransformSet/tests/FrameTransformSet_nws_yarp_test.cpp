/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <iostream>

#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("dev::frameTransformSet_nws_yarp", "[yarp::dev]")
{
    #if defined(DISABLE_FAILING_TESTS)
        YARP_SKIP_TEST("Skipping failing tests")
    #endif

    YARP_REQUIRE_PLUGIN("frameTransformSet_nws_yarp", "device");

    yarp::os::Network::setLocalMode(true);
    const bool verboseDebug = true;

    SECTION("test the frameTransformSet_nws_yarp alone")
    {
        yarp::dev::PolyDriver pd;
        yarp::os::Property p;

        p.put("device", "frameTransformSet_nws_yarp");
        REQUIRE(pd.open(p));

        yarp::os::Time::delay(0.5);

        REQUIRE(pd.close());
    }

    SECTION("test the frameTransformSet_nws_yarp")
    {
        yarp::dev::PolyDriver pd_nws;
        yarp::dev::PolyDriver pd_dev;
        yarp::os::Property p_nws;
        yarp::os::Property p_dev;

        p_dev.put("device", "frameTransformStorage");
        REQUIRE(pd_dev.open(p_dev));
        p_nws.put("device", "frameTransformSet_nws_yarp");
        REQUIRE(pd_nws.open(p_nws));

        yarp::os::Time::delay(0.5);

        {yarp::dev::WrapperSingle* ww_nws; pd_nws.view(ww_nws);
        bool result_att = ww_nws->attach(&pd_dev);
        REQUIRE(result_att); }

        yarp::os::Time::delay(0.5);

        REQUIRE(pd_nws.close());
        REQUIRE(pd_dev.close());
    }

    yarp::os::Network::setLocalMode(false);
}
