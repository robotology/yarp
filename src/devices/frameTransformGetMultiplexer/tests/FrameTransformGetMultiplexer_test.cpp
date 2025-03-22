/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <iostream>

#include <yarp/dev/WrapperMultiple.h>

#include <yarp/dev/tests/IFrameTransformStorageTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("dev::frameTransformGetMultiplexer", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("frameTransformGetMultiplexer", "device");
    YARP_REQUIRE_PLUGIN("frameTransformStorage", "device");

    yarp::os::Network::setLocalMode(true);
    const bool verboseDebug = true;

    SECTION("test the frameTransformGetMultiplexer")
    {
        yarp::dev::PolyDriver pd_multi;
        yarp::dev::PolyDriver pd_dev1;
        yarp::dev::PolyDriver pd_dev2;
        yarp::os::Property p_multi;
        yarp::os::Property p_dev1;
        yarp::os::Property p_dev2;

        p_dev1.put("device", "frameTransformStorage");
        REQUIRE(pd_dev1.open(p_dev1));
        p_dev2.put("device", "frameTransformStorage");
        REQUIRE(pd_dev2.open(p_dev2));

        p_multi.put("device", "frameTransformGetMultiplexer");
        REQUIRE(pd_multi.open(p_multi));

        yarp::os::Time::delay(0.5);

        {yarp::dev::WrapperMultiple* ww_multi=nullptr; pd_multi.view(ww_multi);
        REQUIRE(ww_multi);
        PolyDriverList pl;
        pl.push(&pd_dev1, "storage1");
        pl.push(&pd_dev2, "storage1");
        bool result_att = ww_multi->attachAll(pl);
        REQUIRE(result_att); }

        IFrameTransformStorageGet* itf_get = nullptr;
        IFrameTransformStorageSet* itf_set = nullptr;
        IFrameTransformStorageUtils* itf_utils = nullptr;
        yarp::os::Time::delay(0.5);

        REQUIRE(pd_multi.view(itf_get));

        yarp::dev::tests::exec_frameTransformStorage_test_1(itf_set, itf_get, itf_utils);
        REQUIRE(pd_multi.close());
        REQUIRE(pd_dev1.close());
        REQUIRE(pd_dev2.close());
    }

    yarp::os::Network::setLocalMode(false);
}
