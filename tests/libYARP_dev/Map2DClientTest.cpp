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


TEST_CASE("dev::Map2DClientTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("map2DServer", "device");
    YARP_REQUIRE_PLUGIN("map2DClient", "device");
    YARP_REQUIRE_PLUGIN("map2D_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("map2DStorage", "device");

    Network::setLocalMode(true);

    SECTION("Checking INavigation2D methods")
    {
        PolyDriver ddmapserver;
        PolyDriver ddmapclient;
        IMap2D* imap = nullptr;

        ////////"Checking opening map2DServer and map2DClient polydrivers"
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
        }

        //////////"Checking INavigation2D methods
        std::vector<std::string> ll;
        std::vector<std::string> la;
        std::vector<std::string> lp;
        {
            Map2DLocation l1 = Map2DLocation("map_1", 10.0, 20.0, 15);
            Map2DLocation l2 = Map2DLocation("map_1", 10.2, 20.1, 15.5);
            Map2DArea a1("map_1", std::vector<Map2DLocation> {Map2DLocation("map_1", -10, -10, 0),
                Map2DLocation("map_1", -10, +10, 0),
                Map2DLocation("map_1", +10, +10, 0),
                Map2DLocation("map_1", +10, -10, 0)});
            Map2DPath p1(std::vector<Map2DLocation> {l1, l2});
            bool b;
            b = imap->storeArea("area_test1", a1); CHECK(b);
            b = imap->storeLocation("loc_test1", l1); CHECK(b);
            b = imap->storeLocation("loc_test2", l2); CHECK(b);
            b = imap->storePath("path_test1", p1); CHECK(b);
            b = imap->getLocationsList(ll); CHECK(b); CHECK(ll.size() == 2);
            b = imap->getAreasList(la); CHECK(b); CHECK(la.size() == 1);
            b = imap->getPathsList(lp); CHECK(b); CHECK(lp.size() == 1);
        }

        {
            bool b;
            b = imap->saveLocationsAndExtras("locations_test.ini"); CHECK(b);
            b = imap->clearAllLocations(); CHECK(b);
            b = imap->clearAllAreas(); CHECK(b);
            b = imap->clearAllPaths(); CHECK(b);
            b = imap->getLocationsList(ll); CHECK(b); CHECK(ll.size()==0);
            b = imap->getAreasList(la); CHECK(b); CHECK(la.size() == 0);
            b = imap->getPathsList(lp); CHECK(b); CHECK(lp.size() == 0);

            b = imap->loadLocationsAndExtras("locations_test.ini"); CHECK(b);
            Map2DLocation l1t;
            Map2DLocation l2t;
            Map2DArea a1;
            Map2DPath p1;
            b = imap->getLocationsList(ll); CHECK(b); CHECK(ll.size() == 2);
            b = imap->getAreasList(la); CHECK(b); CHECK(la.size() == 1);
            b = imap->getPathsList(lp); CHECK(b); CHECK(lp.size() == 1);

            b = imap->getLocation("loc_test1" ,l1t); CHECK(b);
            b = imap->getLocation("loc_test2", l2t); CHECK(b);
            b = imap->getArea("area_test1", a1); CHECK(b);
            b = imap->getPath("path_test1", p1); CHECK(b);
        }

        //"Close all polydrivers and check"
        {
            CHECK(ddmapclient.close());
            CHECK(ddmapserver.close());
        }
    }

    Network::setLocalMode(false);
}
