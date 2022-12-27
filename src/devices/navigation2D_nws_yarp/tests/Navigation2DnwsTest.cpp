/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/INavigation2DTest.h>

#include <catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::Navigation2DNwsTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("navigation2D_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking navigation2D_nws_yarp device")
    {
        PolyDriver ddnavserver;

        ////////"Checking opening navigation2D_nws_yarp polydrivers"
        {
            Property pnavserver_cfg;
            pnavserver_cfg.put("device", "navigation2D_nws_yarp");
            REQUIRE(ddnavserver.open(pnavserver_cfg));
        }

        //"Close all polydrivers and check"
        {
            CHECK(ddnavserver.close());
        }
    }

    Network::setLocalMode(false);
}
