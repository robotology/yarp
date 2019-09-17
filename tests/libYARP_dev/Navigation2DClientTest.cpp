/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::Navigation2DClientTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("map2DServer", "device");
    YARP_REQUIRE_PLUGIN("map2DClient", "device");
    YARP_REQUIRE_PLUGIN("localization2DServer", "device");
    YARP_REQUIRE_PLUGIN("fakeLocalizer", "device");
    YARP_REQUIRE_PLUGIN("navigation2DServer", "device");
    YARP_REQUIRE_PLUGIN("fakeNavigation", "device");
    YARP_REQUIRE_PLUGIN("navigation2DClient", "device");

    Network::setLocalMode(true);

    SECTION("Checking INavigation2D methods")
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
            pmapserver_cfg.put("device", "map2DServer");
            REQUIRE(ddmapserver.open(pmapserver_cfg));

            Property pmapclient_cfg;
            pmapclient_cfg.put("device", "map2DClient");
            pmapclient_cfg.put("local", "/mapClientTest");
            pmapclient_cfg.put("remote", "/mapServer");
            REQUIRE(ddmapclient.open(pmapclient_cfg));
            REQUIRE(ddmapclient.view(imap));

            Property plocserver_cfg;
            plocserver_cfg.put("device", "localization2DServer");
            plocserver_cfg.put("subdevice", "fakeLocalizer");
            REQUIRE(ddlocserver.open(plocserver_cfg));

            Property pnavserver_cfg;
            pnavserver_cfg.put("device", "navigation2DServer");
            plocserver_cfg.put("subdevice", "fakeNavigation");
            REQUIRE(ddnavserver.open(pnavserver_cfg));

            Property pnavclient_cfg;
            pnavclient_cfg.put("device", "navigation2DClient");
            pnavclient_cfg.put("local", "/navigationClientTest");
            pnavclient_cfg.put("navigation_server", "/navigationServer");
            pnavclient_cfg.put("map_locations_server", "/mapServer");
            pnavclient_cfg.put("localization_server", "/localizationServer");
            REQUIRE(ddnavclient.open(pnavclient_cfg));
            REQUIRE(ddnavclient.view(inav));
        }

        //////////"Checking INavigation2D methods
        {
            Map2DLocation loc_test = Map2DLocation("map_1", 10.0, 20.0, 15);
            Map2DLocation my_current_loc = Map2DLocation("map_1", 10.2, 20.1, 15.5);
            Map2DLocation loc_to_be_tested;
            Map2DArea area_test("map_1", std::vector<Map2DLocation> {Map2DLocation("map_1", -10, -10, 0),
                Map2DLocation("map_1", -10, +10, 0),
                Map2DLocation("map_1", +10, +10, 0),
                Map2DLocation("map_1", +10, -10, 0)});
            bool b0, b1;
            b0 = imap->storeArea("area_test", area_test); CHECK(b0);
            b0 = imap->storeLocation("loc_test", loc_test); CHECK(b0);

            {
                b0 = inav->setInitialPose(my_current_loc); CHECK(b0);
                yarp::os::Time::delay(0.1);
                b0 = inav->getCurrentPosition(loc_to_be_tested); CHECK(b0); CHECK(loc_to_be_tested == my_current_loc);
                b1 = inav->checkInsideArea("area_test");  CHECK(b1 == false);
                b1 = inav->checkInsideArea(area_test);    CHECK(b1 == false);
                b0 = inav->setInitialPose(Map2DLocation("map_1", 0, 0, 0)); CHECK(b0);
                yarp::os::Time::delay(0.1);
                b0 = inav->getCurrentPosition(loc_to_be_tested); CHECK(loc_to_be_tested == Map2DLocation("map_1", 0, 0, 0));
                b1 = inav->checkInsideArea("area_test");  CHECK(b1);
                b1 = inav->checkInsideArea(area_test);    CHECK(b1);
            }

            {
                double lin_tol = 1.0; //m
                double ang_tol = 1.0; //deg
                b0 = inav->setInitialPose(my_current_loc); CHECK(b0);
                yarp::os::Time::delay(0.1);
                b1 = inav->checkNearToLocation("loc_test", lin_tol, ang_tol); CHECK(b1);
                b1 = inav->checkNearToLocation(loc_test, lin_tol, ang_tol); CHECK(b1);
            }
            {
                double lin_tol = 0.0001; //m
                double ang_tol = 0.0001; //deg
                b0 = inav->setInitialPose(my_current_loc); CHECK(b0);
                yarp::os::Time::delay(0.1);
                b1 = inav->checkNearToLocation("loc_test", lin_tol, ang_tol); CHECK(b1==false);
                b1 = inav->checkNearToLocation(loc_test, lin_tol, ang_tol); CHECK(b1==false);
            }
            {
                double lin_tol = 1.0; //m
                Map2DLocation my_current_loc2 = my_current_loc; my_current_loc2.theta = 90;
                b0 = inav->setInitialPose(my_current_loc2); CHECK(b0);
                yarp::os::Time::delay(0.1);
                b1 = inav->checkNearToLocation("loc_test", lin_tol); CHECK(b1);
                b1 = inav->checkNearToLocation(loc_test, lin_tol); CHECK(b1);
            }
            {
                double lin_tol = 0.1; //m
                double ang_tol = 0.1; //deg
                b0 = inav->setInitialPose(my_current_loc); CHECK(b0); yarp::os::Time::delay(0.1);
                b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5) , lin_tol, ang_tol); CHECK(b1);
                b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5 + 180), lin_tol, ang_tol); CHECK(b1 == false);
                b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5 + 360), lin_tol, ang_tol); CHECK(b1);
                b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5 + 720), lin_tol, ang_tol); CHECK(b1);
                b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5 - 180), lin_tol, ang_tol); CHECK(b1 == false);
                b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5 - 360), lin_tol, ang_tol); CHECK(b1);
                b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5 - 720), lin_tol, ang_tol); CHECK(b1);
            }
        }

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
