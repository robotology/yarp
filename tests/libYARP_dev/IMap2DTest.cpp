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
#include "IMap2DTest.h"

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;

void exec_iMap2D_test_1(IMap2D* imap)
{
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
        b = imap->getLocationsList(ll); CHECK(b); CHECK(ll.size() == 0);
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

        b = imap->getLocation("loc_test1", l1t); CHECK(b);
        b = imap->getLocation("loc_test2", l2t); CHECK(b);
        b = imap->getArea("area_test1", a1); CHECK(b);
        b = imap->getPath("path_test1", p1); CHECK(b);
    }
}

void exec_iMap2D_test_2(IMap2D* imap)
{
    //////////"Checking IMap2D methods which involve usage of classes Map2DArea or Map2DLocation"
    {
        std::vector <Map2DArea>      areas;
        std::vector <Map2DLocation>  locs;
        std::vector <std::string>    loc_names;
        Map2DLocation loc;
        Map2DArea     area;
        bool ret;
        bool b1;
        ret = imap->clearAllLocations();   CHECK(ret);
        ret = imap->clearAllAreas();       CHECK(ret);
        ret = imap->getAreasList(loc_names);   CHECK(ret);
        b1 = (loc_names.size() == 0);
        CHECK(b1);
        ret = imap->getLocationsList(loc_names);   CHECK(ret);
        b1 = (loc_names.size() == 0);
        CHECK(b1);

        ret = imap->storeLocation("loc1", Map2DLocation("map1", 1, 2, 3));  CHECK(ret);
        ret = imap->storeLocation("loc2", Map2DLocation("map2", 4, 5, 6));  CHECK(ret);

        std::vector<Map2DLocation> vec1, vec2, vec3;
        Map2DLocation v;
        v.x = 1.1; v.y = 1.1; vec1.push_back(v);
        v.x = 1.2; v.y = 1.2; vec1.push_back(v);
        v.x = 1.3; v.y = 1.3; vec1.push_back(v);
        ret = imap->storeArea("area1", Map2DArea("map1", vec1));  CHECK(ret);
        v.x = 2.1; v.y = 2.1; vec2 = vec1;  vec2.push_back(v);
        ret = imap->storeArea("area2", Map2DArea("map2", vec2));  CHECK(ret);
        v.x = 3.1; v.y = 3.1; vec3 = vec2;  vec3.push_back(v);
        ret = imap->storeArea("area3", Map2DArea("map3", vec3));  CHECK(ret);

        ret = imap->getAreasList(loc_names);   CHECK(ret);
        b1 = (loc_names.size() == 3);
        CHECK(b1);
        ret = imap->getLocationsList(loc_names);  CHECK(ret);
        b1 = (loc_names.size() == 2);
        CHECK(b1);

        ret = imap->getArea("area1", area);   CHECK(ret);
        b1 = (area == Map2DArea("map1", vec1));
        CHECK(b1);
        ret = imap->getArea("area_err", area);   CHECK(ret == false);

        ret = imap->getLocation("loc1", loc);  CHECK(ret);
        b1 = (loc == Map2DLocation("map1", 1, 2, 3));
        CHECK(b1);
        ret = imap->getLocation("loc_err", loc);   CHECK(ret == false);

        ret = imap->deleteArea("area1");       CHECK(ret);
        ret = imap->deleteLocation("loc1");  CHECK(ret);
        ret = imap->deleteArea("area_err");       CHECK(ret == false);
        ret = imap->deleteLocation("loc_err");  CHECK(ret == false);
        ret = imap->getArea("area1", area);    CHECK(ret == false);
        ret = imap->getLocation("loc1", loc);  CHECK(ret == false);

        ret = imap->getArea("area2", area);  CHECK(ret);
        b1 = (area == Map2DArea("map2", vec2));
        CHECK(b1);
        ret = imap->getLocation("loc2", loc);   CHECK(ret);
        b1 = (loc == Map2DLocation("map2", 4, 5, 6));
        CHECK(b1);

        ret = imap->clearAllLocations();  CHECK(ret);
        ret = imap->clearAllAreas();  CHECK(ret);
        ret = imap->getAreasList(loc_names);  CHECK(ret);
        b1 = (loc_names.size() == 0);
        CHECK(b1);
        imap->getLocationsList(loc_names);
        b1 = (loc_names.size() == 0);
        CHECK(b1);

        ret = imap->storeArea("area", Map2DArea("map1", vec1));  CHECK(ret);
        ret = imap->storeLocation("loc", Map2DLocation("map2", 4, 5, 6));  CHECK(ret);
        ret = imap->renameArea("area_fail", "area_new");  CHECK(ret == false);
        ret = imap->renameLocation("loc_fail", "loc_new");  CHECK(ret == false);
        ret = imap->renameArea("area", "area_new");  CHECK(ret);
        ret = imap->renameLocation("loc", "loc_new");  CHECK(ret);
        ret = imap->getArea("area", area);         CHECK(ret == false);
        ret = imap->getArea("area_new", area);      CHECK(ret);
        ret = imap->getLocation("loc", loc);       CHECK(ret == false);
        ret = imap->getLocation("loc_new", loc);    CHECK(ret);

        //final cleanup, already tested
        ret = imap->clearAllLocations();  CHECK(ret);
        ret = imap->clearAllAreas();  CHECK(ret);
    }

    //////////"Checking IMap2D methods which involve usage of classes Map2DPath/Map2DLocation"
    {
        std::vector <Map2DPath>      paths;
        std::vector <Map2DLocation>  locs;
        std::vector <std::string>    loc_names;
        std::vector <std::string>    path_names;
        Map2DLocation loc;
        Map2DPath     path;
        bool ret;
        bool b1;
        ret = imap->clearAllLocations();   CHECK(ret);
        ret = imap->clearAllPaths();       CHECK(ret);
        ret = imap->getPathsList(path_names);   CHECK(ret);
        b1 = (path_names.size() == 0);
        CHECK(b1);
        ret = imap->getLocationsList(loc_names);   CHECK(ret);
        b1 = (loc_names.size() == 0);
        CHECK(b1);

        ret = imap->storeLocation("loc1", Map2DLocation("map1", 1, 2, 3));  CHECK(ret);
        ret = imap->storeLocation("loc2", Map2DLocation("map2", 4, 5, 6));  CHECK(ret);

        std::vector<Map2DLocation> vec1, vec2, vec3;
        Map2DLocation v;
        v.x = 1.1; v.y = 1.1; vec1.push_back(v);
        v.x = 1.2; v.y = 1.2; vec1.push_back(v);
        v.x = 1.3; v.y = 1.3; vec1.push_back(v);
        ret = imap->storePath("path1", Map2DPath(vec1));  CHECK(ret);
        v.x = 2.1; v.y = 2.1; vec2 = vec1;  vec2.push_back(v);
        ret = imap->storePath("path2", Map2DPath(vec2));  CHECK(ret);
        v.x = 3.1; v.y = 3.1; vec3 = vec2;  vec3.push_back(v);
        ret = imap->storePath("path3", Map2DPath(vec3));  CHECK(ret);

        ret = imap->getPathsList(path_names);   CHECK(ret);
        b1 = (path_names.size() == 3);
        CHECK(b1);
        ret = imap->getLocationsList(loc_names);  CHECK(ret);
        b1 = (loc_names.size() == 2);
        CHECK(b1);

        ret = imap->getPath("path1", path);   CHECK(ret);
        b1 = (path == Map2DPath(vec1));
        CHECK(b1);
        ret = imap->getPath("path_err", path);   CHECK(ret == false);

        ret = imap->getLocation("loc1", loc);  CHECK(ret);
        b1 = (loc == Map2DLocation("map1", 1, 2, 3));
        CHECK(b1);
        ret = imap->getLocation("loc_err", loc);   CHECK(ret == false);

        ret = imap->deletePath("path1");       CHECK(ret);
        ret = imap->deleteLocation("loc1");  CHECK(ret);
        ret = imap->deletePath("path_err");       CHECK(ret == false);
        ret = imap->deleteLocation("loc_err");  CHECK(ret == false);
        ret = imap->getPath("path1", path);    CHECK(ret == false);
        ret = imap->getLocation("loc1", loc);  CHECK(ret == false);

        ret = imap->getPath("path2", path);  CHECK(ret);
        b1 = (path == Map2DPath(vec2));
        CHECK(b1);
        ret = imap->getLocation("loc2", loc);   CHECK(ret);
        b1 = (loc == Map2DLocation("map2", 4, 5, 6));
        CHECK(b1);

        ret = imap->clearAllLocations();  CHECK(ret);
        ret = imap->clearAllPaths();  CHECK(ret);
        ret = imap->getPathsList(path_names);  CHECK(ret);
        b1 = (path_names.size() == 0);
        CHECK(b1);
        imap->getLocationsList(loc_names);
        b1 = (loc_names.size() == 0);
        CHECK(b1);

        ret = imap->storePath("path", Map2DPath(vec1));  CHECK(ret);
        ret = imap->storeLocation("loc", Map2DLocation("map2", 4, 5, 6));  CHECK(ret);
        ret = imap->renamePath("path_fail", "path_new");  CHECK(ret == false);
        ret = imap->renameLocation("loc_fail", "loc_new");  CHECK(ret == false);
        ret = imap->renamePath("path", "path_new");  CHECK(ret);
        ret = imap->renameLocation("loc", "loc_new");  CHECK(ret);
        ret = imap->getPath("path", path);         CHECK(ret == false);
        ret = imap->getPath("path_new", path);      CHECK(ret);
        ret = imap->getLocation("loc", loc);       CHECK(ret == false);
        ret = imap->getLocation("loc_new", loc);    CHECK(ret);

        path.toString();

        //final cleanup, already tested
        ret = imap->clearAllLocations();  CHECK(ret);
        ret = imap->clearAllPaths();  CHECK(ret);
    }

    //////////"Checking IMap2D methods which involve usage of classes MapGrid2D"
    {
        Nav2D::MapGrid2D test_store_map1;
        Nav2D::MapGrid2D test_store_map2;
        Nav2D::MapGrid2D test_get_map;

        test_store_map1.setMapName("test_map1");
        test_store_map2.setMapName("test_map2");
        std::vector<std::string> map_names;

        imap->store_map(test_store_map1);
        imap->store_map(test_store_map2);
        imap->get_map("test_map1", test_get_map);
        CHECK(test_store_map1.isIdenticalTo(test_get_map)); // IMap2D store/get operation successful

        imap->get_map_names(map_names);
        bool b1 = (map_names.size() == 2);
        bool b2 = false;
        if (b1) {
            b2 = (map_names[0] == "test_map1" && map_names[1] == "test_map2");
        }
        CHECK(b1);
        CHECK(b2);
        // IMap2D get_map_names operation successful")

        imap->remove_map("test_map1");
        imap->get_map_names(map_names);
        b1 = (map_names.size() == 1);
        CHECK(b1); // IMap2D remove_map operation successful

        imap->clearAllMaps();
        imap->get_map_names(map_names);
        b1 = (map_names.size() == 0);
        CHECK(b1); // IMap2D clear operation successful
    }
}
