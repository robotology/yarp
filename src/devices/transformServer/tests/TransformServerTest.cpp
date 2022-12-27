/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <catch_amalgamated.hpp>
#include <harness.h>

#include <yarp/dev/tests/IFrameTransformTest.h>

using namespace yarp::os;
using namespace yarp::dev;


TEST_CASE("dev::TransformServerTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("transformServer", "device");

    Network::setLocalMode(true);

    SECTION("Test the transform server")
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

        CHECK(ddtransformserver.close()); // ddtransformserver successfully closed
    }

    Network::setLocalMode(false);
}
