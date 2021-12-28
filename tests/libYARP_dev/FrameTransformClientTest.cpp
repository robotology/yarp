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

#include "IFrameTransformTest.h"

#include <catch.hpp>
#include <harness.h>

TEST_CASE("dev::FrameTransformClientTest", "[yarp::dev]")
{
    #if defined(DISABLE_FAILING_TESTS)
        YARP_SKIP_TEST("Skipping failing tests")
    #endif

    YARP_REQUIRE_PLUGIN("frameTransformClient", "device");

    yarp::os::Network::setLocalMode(true);
    const bool verboseDebug = true;

    SECTION("test the frameTransformClient/Server open/close, case 0")
    {
        yarp::dev::PolyDriver pd;
        yarp::os::Property p;

        p.put("device", "frameTransformClient");
        p.put("filexml_option", "ftc_local_only.xml");
        REQUIRE(pd.open(p));
        yarp::os::Time::delay(0.5);
        REQUIRE(pd.close());

        p.put("device", "frameTransformServer");
        p.put("filexml_option", "fts_yarp_only.xml");
        REQUIRE(pd.open(p));
        yarp::os::Time::delay(0.5);
        REQUIRE(pd.close());
    }

    SECTION("test the frameTransformClient local only mode, case 1")
    {
        yarp::dev::IFrameTransform* ift = nullptr;
        yarp::dev::PolyDriver pd;
        yarp::os::Property p;
        p.put("device","frameTransformClient");
        p.put("filexml_option","ftc_local_only.xml");
        if(verboseDebug) {p.put("FrameTransform_verbose_debug","1"); }
        REQUIRE(pd.open(p));
        REQUIRE(pd.view(ift));

        exec_frameTransform_test_1(ift);

        REQUIRE(pd.close());
    }

    SECTION("test the frameTransformClient local only mode, case 2")
    {
        yarp::dev::IFrameTransform* ift = nullptr;
        yarp::dev::PolyDriver pd;
        yarp::os::Property p;
        p.put("device", "frameTransformClient");
        p.put("filexml_option", "ftc_local_only.xml");
        if (verboseDebug) { p.put("FrameTransform_verbose_debug", "1"); }
        REQUIRE(pd.open(p));
        REQUIRE(pd.view(ift));

        exec_frameTransform_test_2(ift);

        REQUIRE(pd.close());
    }

    SECTION("test the frameTransformClient/Server, case 3")
    {
        yarp::dev::PolyDriver server_pd;
        yarp::os::Property server_prop;
        server_prop.put("device", "frameTransformServer");
        server_prop.put("filexml_option", "fts_yarp_only.xml");
        if (verboseDebug) { server_prop.put("FrameTransform_verbose_debug", "1"); }
        REQUIRE(server_pd.open(server_prop));

        yarp::dev::IFrameTransform* ift = nullptr;
        yarp::dev::PolyDriver client_pd;
        yarp::os::Property client_prop;
        client_prop.put("device", "frameTransformClient");
        client_prop.put("filexml_option", "ftc_yarp_only.xml");
        if (verboseDebug) { client_prop.put("FrameTransform_verbose_debug", "1"); }
        REQUIRE(client_pd.open(client_prop));
        REQUIRE(client_pd.view(ift));

        exec_frameTransform_test_1(ift);

        REQUIRE(client_pd.close());
        REQUIRE(server_pd.close());
    }

    SECTION("test the frameTransformClient/Server, case 4")
    {
        yarp::dev::PolyDriver server_pd;
        yarp::os::Property server_prop;
        server_prop.put("device", "frameTransformServer");
        server_prop.put("filexml_option", "fts_yarp_only.xml");
        if (verboseDebug) { server_prop.put("FrameTransform_verbose_debug", "1"); }
        REQUIRE(server_pd.open(server_prop));

        yarp::dev::IFrameTransform* ift = nullptr;
        yarp::dev::PolyDriver client_pd;
        yarp::os::Property client_prop;
        client_prop.put("device", "frameTransformClient");
        client_prop.put("filexml_option", "ftc_yarp_only.xml");
        if (verboseDebug) { client_prop.put("FrameTransform_verbose_debug", "1"); }
        REQUIRE(client_pd.open(client_prop));
        REQUIRE(client_pd.view(ift));

        exec_frameTransform_test_2(ift);

        REQUIRE(client_pd.close());
        REQUIRE(server_pd.close());
    }

    yarp::os::Network::setLocalMode(false);
}
