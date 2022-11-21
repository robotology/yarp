/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IMap2DTest.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::Map2DnwsTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("map2D_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking map2D_nws_yarp device")
    {
        PolyDriver ddmapserver;

        ////////"Checking opening map2DServer and map2DClient polydrivers"
        {
            Property pmapserver_cfg;
            pmapserver_cfg.put("device", "map2D_nws_yarp");
            REQUIRE(ddmapserver.open(pmapserver_cfg));
        }

        //"Close all polydrivers and check"
        {
            CHECK(ddmapserver.close());
        }
    }

    Network::setLocalMode(false);
}
