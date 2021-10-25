/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <catch.hpp>
#include <harness.h>

#include "IFrameTransformTest.h"

using namespace yarp::os;
using namespace yarp::dev;


TEST_CASE("dev::TransformClientTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("transformServer", "device");
    YARP_REQUIRE_PLUGIN("transformClient", "device");

    Network::setLocalMode(true);

#if defined(ENABLE_BROKEN_TESTS)
    SECTION("Test the transform client, test 1")
    {
        bool precision_verbose = false;

        PolyDriver ddtransformserver;
        Property pTransformserver_cfg;
        pTransformserver_cfg.put("device", "transformServer");
        Property& ros_prop = pTransformserver_cfg.addGroup("ROS");
        ros_prop.put("enable_ros_publisher", "0");
        ros_prop.put("enable_ros_subscriber", "0");
        pTransformserver_cfg.put("transforms_lifetime", 0.500);
        bool ok_server = ddtransformserver.open(pTransformserver_cfg);
        CHECK(ok_server); // ddtransformserver open reported successful

        IFrameTransform* itf = nullptr;
        PolyDriver ddtransformclient;
        Property pTransformclient_cfg;
        pTransformclient_cfg.put("device", "transformClient");
        pTransformclient_cfg.put("local", "/transformClientTest");
        pTransformclient_cfg.put("remote", "/transformServer");
        bool ok_client = ddtransformclient.open(pTransformclient_cfg);
        CHECK(ok_client); // ddtransformclient open reported successful

        bool ok_view = ddtransformclient.view(itf);
        REQUIRE(ok_view);
        REQUIRE(itf != nullptr); // iTransform interface open reported successful

        //execute the test
        exec_frameTransform_test_1(itf);

        // Close devices
        CHECK(ddtransformclient.close()); // ddtransformclient successfully closed
        CHECK(ddtransformserver.close()); // ddtransformserver successfully closed
    }

    SECTION("Test the transform client, test 2")
    {
        bool precision_verbose = false;

        PolyDriver ddtransformserver;
        Property pTransformserver_cfg;
        pTransformserver_cfg.put("device", "transformServer");
        Property& ros_prop = pTransformserver_cfg.addGroup("ROS");
        ros_prop.put("enable_ros_publisher", "0");
        ros_prop.put("enable_ros_subscriber", "0");
        pTransformserver_cfg.put("transforms_lifetime", 0.500);
        bool ok_server = ddtransformserver.open(pTransformserver_cfg);
        CHECK(ok_server); // ddtransformserver open reported successful

        IFrameTransform* itf = nullptr;
        PolyDriver ddtransformclient;
        Property pTransformclient_cfg;
        pTransformclient_cfg.put("device", "transformClient");
        pTransformclient_cfg.put("local", "/transformClientTest");
        pTransformclient_cfg.put("remote", "/transformServer");
        bool ok_client = ddtransformclient.open(pTransformclient_cfg);
        CHECK(ok_client); // ddtransformclient open reported successful

        bool ok_view = ddtransformclient.view(itf);
        REQUIRE(ok_view);
        REQUIRE(itf != nullptr); // iTransform interface open reported successful

        //execute the test
        //exec_frameTransform_test_2(itf);

        // Close devices
        CHECK(ddtransformclient.close()); // ddtransformclient successfully closed
        CHECK(ddtransformserver.close()); // ddtransformserver successfully closed
    }

#endif
    Network::setLocalMode(false);
}
