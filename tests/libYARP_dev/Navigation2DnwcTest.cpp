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

#include <catch.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;

#include "INavigation2DTest.h"

TEST_CASE("dev::Navigation2DNwcTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("map2D_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("map2D_nwc_yarp", "device");
    YARP_REQUIRE_PLUGIN("map2DStorage", "device");
    YARP_REQUIRE_PLUGIN("localization2D_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("fakeLocalizer", "device");
    YARP_REQUIRE_PLUGIN("navigation2D_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("fakeNavigation", "device");
    YARP_REQUIRE_PLUGIN("navigation2D_nwc_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking navigation2D_nwc_yarp < -> navigation2D_nws_yarp communication and yarp::dev::Nav2D::INavigation2D methods")
    {
        PolyDriver ddnavserver;
        PolyDriver ddmapserver;
        PolyDriver ddmapclient;
        PolyDriver ddlocserver;
        PolyDriver ddnavclient;
        INavigation2D* inav = nullptr;
        IMap2D* imap = nullptr;

        ////////"Checking opening navigation2DServer and navigation2DClient polydrivers"
        {
            Property pmapserver_cfg;
            pmapserver_cfg.put("device", "map2D_nws_yarp");
            pmapserver_cfg.put("subdevice", "map2DStorage");
            REQUIRE(ddmapserver.open(pmapserver_cfg));

            Property pmapclient_cfg;
            pmapclient_cfg.put("device", "map2D_nwc_yarp");
            pmapclient_cfg.put("local", "/mapClientTest");
            pmapclient_cfg.put("remote", "/map2D_nws_yarp");
            REQUIRE(ddmapclient.open(pmapclient_cfg));
            REQUIRE(ddmapclient.view(imap));

            Property plocserver_cfg;
            plocserver_cfg.put("device", "localization2D_nws_yarp");
            plocserver_cfg.put("subdevice", "fakeLocalizer");
            REQUIRE(ddlocserver.open(plocserver_cfg));

            Property pnavserver_cfg;
            pnavserver_cfg.put("device", "navigation2D_nws_yarp");
            pnavserver_cfg.put("subdevice", "fakeNavigation");
            REQUIRE(ddnavserver.open(pnavserver_cfg));

            Property pnavclient_cfg;
            pnavclient_cfg.put("device", "navigation2D_nwc_yarp");
            pnavclient_cfg.put("local", "/navigationClientTest");
            pnavclient_cfg.put("navigation_server", "/navigation2D_nws_yarp");
            pnavclient_cfg.put("map_locations_server", "/map2D_nws_yarp");
            pnavclient_cfg.put("localization_server", "/localization2D_nws_yarp");
            REQUIRE(ddnavclient.open(pnavclient_cfg));
            REQUIRE(ddnavclient.view(inav));
        }

        // Do tests
        exec_iNav2D_test_1(inav, imap);
        exec_iNav2D_test_2(inav, imap);

        //"Close all polydrivers and check"
        {
            CHECK(ddnavclient.close());
            CHECK(ddnavserver.close());
            CHECK(ddlocserver.close());
            CHECK(ddmapclient.close());
            CHECK(ddmapserver.close());
        }
    }

    Network::setLocalMode(false);
}
