/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <iostream>

#include <yarp/dev/tests/IFrameTransformStorageTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("dev::frameTransformStorage", "[yarp::dev]")
{
    #if defined(DISABLE_FAILING_TESTS)
        YARP_SKIP_TEST("Skipping failing tests")
    #endif

    YARP_REQUIRE_PLUGIN("frameTransformStorage", "device");

    yarp::os::Network::setLocalMode(true);
    const bool verboseDebug = true;

    SECTION("test the frameTransformStorage")
    {
        yarp::dev::PolyDriver pd;
        yarp::os::Property p;

        p.put("device", "frameTransformStorage");
        REQUIRE(pd.open(p));

        IFrameTransformStorageGet*   itf_get = nullptr;
        IFrameTransformStorageSet*   itf_set = nullptr;
        IFrameTransformStorageUtils* itf_utils = nullptr;

        yarp::os::Time::delay(0.5);

        REQUIRE(pd.view(itf_get));
        REQUIRE(pd.view(itf_set));
        REQUIRE(pd.view(itf_utils));

        yarp::dev::tests::exec_frameTransformStorage_test_1(itf_set, itf_get, itf_utils);

        REQUIRE(pd.close());
    }

    yarp::os::Network::setLocalMode(false);
}
