/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <iostream>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("dev::DeviceBundlerTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("deviceBundler", "device");
    YARP_REQUIRE_PLUGIN("fakeBattery", "device");
    YARP_REQUIRE_PLUGIN("battery_nws_yarp", "device");

    yarp::os::Network::setLocalMode(true);
    const bool verboseDebug = true;

    SECTION("Test the DeviceBundler")
    {
        yarp::dev::PolyDriver pd;
        yarp::os::Property p;

        p.put("device", "deviceBundler");
        p.put("wrapper_device", "battery_nws_yarp");
        p.put("name", "/battery_nws_yarp");
        p.put("attached_device", "fakeBattery");
        REQUIRE(pd.open(p));
        yarp::os::Time::delay(1.0);
        REQUIRE(pd.close());
    }

    yarp::os::Network::setLocalMode(false);
}
