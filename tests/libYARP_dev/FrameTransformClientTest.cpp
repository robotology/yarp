/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>
#include <yarp/math/FrameTransform.h>
#include <thread>
#include <iostream>
#include <math.h>

#include "IFrameTransformTest.h"

#include <catch.hpp>
#include <harness.h>

TEST_CASE("dev::FrameTransformClientTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("frameTransformClient", "device");

    yarp::os::Network::setLocalMode(true);

    SECTION("test the frameTransformClient local only mode, case 1")
    {
        yarp::dev::IFrameTransform* ift;
        yarp::dev::PolyDriver pd;
        yarp::os::Property p;
        p.put("device","frameTransformClient");
        p.put("filexml_option","ftc_local_only.xml");
        REQUIRE(pd.open(p));
        REQUIRE(pd.view(ift));

        exec_frameTransform_test_1(ift);

        REQUIRE(pd.close());
    }

    SECTION("test the frameTransformClient local only mode, case 2")
    {
        yarp::dev::IFrameTransform* ift;
        yarp::dev::PolyDriver pd;
        yarp::os::Property p;
        p.put("device", "frameTransformClient");
        p.put("filexml_option", "ftc_local_only.xml");
        REQUIRE(pd.open(p));
        REQUIRE(pd.view(ift));

        exec_frameTransform_test_2(ift);

        REQUIRE(pd.close());
    }

    yarp::os::Network::setLocalMode(false);
}
