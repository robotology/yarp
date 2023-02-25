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

#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IFrameTransformStorageTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("dev::frameTransformSet_nwc_yarp", "[yarp::dev]")
{
    #if defined(DISABLE_FAILING_TESTS)
        YARP_SKIP_TEST("Skipping failing tests")
    #endif

    YARP_REQUIRE_PLUGIN("frameTransformSet_nwc_yarp", "device");

    yarp::os::Network::setLocalMode(true);
    const bool verboseDebug = true;

    SECTION("test the frameTransformSet_nwc_yarp")
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

        yarp::dev::PolyDriver pd_nwc;
        yarp::os::Property p_nwc;

        p_nwc.put("device", "frameTransformSet_nwc_yarp");
        REQUIRE(pd_nwc.open(p_nwc));

        IFrameTransformStorageGet*   itf_get = nullptr;
        IFrameTransformStorageSet*   itf_set = nullptr;
        IFrameTransformStorageUtils* itf_utils = nullptr;

        yarp::os::Time::delay(0.5);

        REQUIRE(pd_nwc.view(itf_set));

        yarp::dev::tests::exec_frameTransformStorage_test_1(itf_set, itf_get, itf_utils);

        REQUIRE(pd_nwc .close());
    }

    yarp::os::Network::setLocalMode(false);
}
