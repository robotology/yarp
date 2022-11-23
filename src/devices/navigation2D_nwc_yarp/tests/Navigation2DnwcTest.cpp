/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/INavigation2DTest.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;

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
        PolyDriver ddmapstorage;
        PolyDriver ddmapclient;
        PolyDriver ddlocserver;
        PolyDriver ddnavclient;
        PolyDriver ddfakeLocalizer;
        PolyDriver ddfakeNavigation;
        INavigation2D* inav = nullptr;
        IMap2D* imap = nullptr;

        ////////"Checking opening navigation2DServer and navigation2DClient polydrivers"
        {
            Property pmapserver_cfg;
            pmapserver_cfg.put("device", "map2D_nws_yarp");
            REQUIRE(ddmapserver.open(pmapserver_cfg));
            Property pmapstorage_cfg;
            pmapstorage_cfg.put("device", "map2DStorage");
            REQUIRE(ddmapstorage.open(pmapstorage_cfg));
            {yarp::dev::WrapperSingle* ww_nws; ddmapserver.view(ww_nws);
            bool result_att = ww_nws->attach(&ddmapstorage); }

            Property pmapclient_cfg;
            pmapclient_cfg.put("device", "map2D_nwc_yarp");
            pmapclient_cfg.put("local", "/mapClientTest");
            pmapclient_cfg.put("remote", "/map2D_nws_yarp");
            REQUIRE(ddmapclient.open(pmapclient_cfg));
            REQUIRE(ddmapclient.view(imap));

            Property plocserver_cfg;
            plocserver_cfg.put("device", "localization2D_nws_yarp");
            REQUIRE(ddlocserver.open(plocserver_cfg));
            Property pfakeLocalizer_cfg;
            pfakeLocalizer_cfg.put("device", "fakeLocalizer");
            REQUIRE(ddfakeLocalizer.open(pfakeLocalizer_cfg));
            {yarp::dev::WrapperSingle* ww_nws; ddlocserver.view(ww_nws);
            bool result_att = ww_nws->attach(&ddfakeLocalizer); }

            Property pnavserver_cfg;
            pnavserver_cfg.put("device", "navigation2D_nws_yarp");
            REQUIRE(ddnavserver.open(pnavserver_cfg));
            Property pfakeNavigation;
            pfakeNavigation.put("device", "fakeNavigation");
            REQUIRE(ddfakeNavigation.open(pfakeNavigation));
            {yarp::dev::WrapperSingle* ww_nws; ddnavserver.view(ww_nws);
            bool result_att = ww_nws->attach(&ddfakeNavigation); }

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
        yarp::dev::tests::exec_iNav2D_test_1(inav, imap);
        yarp::dev::tests::exec_iNav2D_test_2(inav, imap);

        //"Close all polydrivers and check"
        {
            CHECK(ddnavclient.close());
            CHECK(ddnavserver.close());
            CHECK(ddlocserver.close());
            CHECK(ddmapclient.close());
            CHECK(ddmapserver.close());
            CHECK(ddmapstorage.close());
            CHECK(ddfakeLocalizer.close());
            CHECK(ddfakeNavigation.close());
        }
    }

    Network::setLocalMode(false);
}
