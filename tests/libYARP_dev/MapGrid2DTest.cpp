/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

static void ReadMapfromString(MapGrid2D& m, std::string s)
{
    yarp::dev::MapGrid2D::XYCell cell;
    for (unsigned int i = 0; i < s.length(); i++)
    {
        if (s.at(i) == '\n') { cell.x = 0; cell.y++; continue; }
        yarp::dev::MapGrid2D::map_flags flag;
        if (s.at(i) == '*') flag = yarp::dev::MapGrid2D::map_flags::MAP_CELL_UNKNOWN;
        else if (s.at(i) == '.') flag = yarp::dev::MapGrid2D::map_flags::MAP_CELL_FREE;
        else if (s.at(i) == '#') flag = yarp::dev::MapGrid2D::map_flags::MAP_CELL_WALL;
        else flag = yarp::dev::MapGrid2D::map_flags::MAP_CELL_UNKNOWN;
        m.setMapFlag(cell, flag);
        cell.x++;
    }
}

TEST_CASE("dev::MapGrid2DTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("map2DServer", "device");
    YARP_REQUIRE_PLUGIN("map2DClient", "device");

    Network::setLocalMode(true);

    SECTION("Test data type MapGrid2D")
    {
        MapGrid2D test_map;
        test_map.setResolution(1.0);
        test_map.setSize_in_meters(11, 11);
        test_map.setOrigin(-3, -7, 0);

        //This is the test map
        ///// 1 3 5 7 9
        //// 0 2 4 6 8 0
        std::string mapstring(
            "***********\n"\
            "#########**\n"\
            "#.......#**\n"\
            "#....######\n"\
            "#....#....#\n"\
            "#....#....#\n"\
            "#....###..#\n"\
            "##...#....#\n"\
            "*#...#....#\n"\
            "*#........#\n"\
            "*##########\n");
        ReadMapfromString(test_map, mapstring);

        MapGrid2D test_cnvutils_map1 = test_map;
        yarp::dev::MapGrid2D::XYCell cell1;
        yarp::dev::MapGrid2D::XYCell cell1_test(6, 4);
        yarp::dev::MapGrid2D::XYWorld world1(3, -1);
        cell1 = test_cnvutils_map1.world2Cell(world1);
        CHECK(cell1 == cell1_test); // IMap2D world2Cell() operation successful

        MapGrid2D test_cnvutils_map2 = test_map;
        yarp::dev::MapGrid2D::XYCell cell2(6, 4);
        yarp::dev::MapGrid2D::XYWorld world2;
        yarp::dev::MapGrid2D::XYWorld world2_test(3, -1);
        world2 = test_cnvutils_map2.cell2World(cell2);
        CHECK(world2 == world2_test); // IMap2D cell2World() operation successful

        MapGrid2D test_cnvutils_map3 = test_map;
        yarp::dev::MapGrid2D::XYCell cell3_ok(0, 0);
        yarp::dev::MapGrid2D::XYWorld world3_ok(-1, -1);
        yarp::dev::MapGrid2D::XYCell cell4_err(11, 11);
        yarp::dev::MapGrid2D::XYWorld world4_err(100, 100);
        yarp::dev::MapGrid2D::XYCell cell5_err(-1, -1);
        yarp::dev::MapGrid2D::XYWorld world5_err(-100, -100);
        CHECK(test_cnvutils_map3.isInsideMap(cell3_ok));
        CHECK(test_cnvutils_map3.isInsideMap(world3_ok));
        CHECK_FALSE(test_cnvutils_map3.isInsideMap(cell4_err));
        CHECK_FALSE(test_cnvutils_map3.isInsideMap(world4_err));
        CHECK_FALSE(test_cnvutils_map3.isInsideMap(cell5_err));
        CHECK_FALSE(test_cnvutils_map3.isInsideMap(world5_err));
        // IMap2D isInsideMap() test successful
    }

    SECTION("Test data type Map2DArea, Map2DLocation")
    {
        bool b;
        bool b2;

        yarp::dev::Map2DArea area1;
        area1.map_id = "maptest";
        area1.points.push_back(yarp::math::Vec2D<double>(-1, -1));
        area1.points.push_back(yarp::math::Vec2D<double>(-1,  1));
        area1.points.push_back(yarp::math::Vec2D<double>( 1,  1));
        area1.points.push_back(yarp::math::Vec2D<double>( 1, -1));
        b = area1.isValid(); //box area
        CHECK(b);

        yarp::dev::Map2DArea area_err;
        area_err.map_id = "maptest";
        area_err.points.push_back(yarp::math::Vec2D<double>(-1, -1));
        area_err.points.push_back(yarp::math::Vec2D<double>(-1, 1));
        b = area_err.isValid(); //incomplete area
        CHECK(b==false);

        yarp::dev::Map2DArea area2;
        area2.map_id = "maptest";
        area2.points.push_back(yarp::math::Vec2D<double>(-1, -1));
        area2.points.push_back(yarp::math::Vec2D<double>(-1, 1));
        area2.points.push_back(yarp::math::Vec2D<double>( 1, -1));
        b = area2.isValid(); //triangular area
        CHECK(b);

        yarp::dev::Map2DArea area3;
        area3.map_id = "maptest";
        area3.points.push_back(yarp::math::Vec2D<double>(-1, -1));
        area3.points.push_back(yarp::math::Vec2D<double>(-1, 1));
        area3.points.push_back(yarp::math::Vec2D<double>(0, -0.5));
        area3.points.push_back(yarp::math::Vec2D<double>(1,  1));
        area3.points.push_back(yarp::math::Vec2D<double>(1, -1));
        b = area3.isValid(); //concave polygon
        CHECK(b);

        yarp::dev::Map2DLocation t1;
        yarp::dev::Map2DLocation t2;
        b = area1.findAreaBounds(t1, t2);   CHECK(b);
        CHECK(t1 == Map2DLocation("maptest", -1, -1, 0));
        CHECK(t2 == Map2DLocation("maptest",  1,  1, 0));

        b = area2.findAreaBounds(t1, t2);   CHECK(b);
        CHECK(t1 == Map2DLocation("maptest", -1, -1, 0));
        CHECK(t2 == Map2DLocation("maptest", 1, 1, 0));

        b = area3.findAreaBounds(t1, t2);   CHECK(b);
        CHECK(t1 == Map2DLocation("maptest", -1, -1, 0));
        CHECK(t2 == Map2DLocation("maptest", 1, 1, 0));

        for (size_t i = 0; i < 100; i++)
        {
            yarp::dev::Map2DLocation rnd;
            b = area1.getRandomLocation(rnd);
            if (b)
            {
                b2 = area1.checkLocationInsideArea(rnd);   CHECK(b2);
            }
            else
            {
                WARN("getRandomLocation may statistically fail. This is normal.");
            }
        }

        for (size_t i = 0; i < 100; i++)
        {
            yarp::dev::Map2DLocation rnd;
            b = area2.getRandomLocation(rnd);
            if (b)
            {
                b2 = area2.checkLocationInsideArea(rnd);   CHECK(b2);
            }
            else
            {
                WARN("getRandomLocation may statistically fail. This is normal.");
            }
        }

        for (size_t i = 0; i < 100; i++)
        {
            yarp::dev::Map2DLocation rnd;
            b = area3.getRandomLocation(rnd);
            if (b)
            {
                b2 = area3.checkLocationInsideArea(rnd);   CHECK(b2);
            }
            else
            {
                WARN("getRandomLocation may statistically fail. This is normal.");
            }
        }

        b = area1.checkLocationInsideArea(Map2DLocation("maptest", 0, 0, 0)); CHECK(b); //inside
        b = area1.checkLocationInsideArea(Map2DLocation("maptest", 1, 1, 0)); CHECK(b == false); //on the vertex
        b = area1.checkLocationInsideArea(Map2DLocation("maptest", 0.999, 0.999, 0)); CHECK(b); //on the vertex
        b = area1.checkLocationInsideArea(Map2DLocation("maptest", 2, 2, 0)); CHECK(b == false); //outside

        b = area2.checkLocationInsideArea(Map2DLocation("maptest", -0.5, -0.5, 0)); CHECK(b); //inside
        b = area2.checkLocationInsideArea(Map2DLocation("maptest", 0, 0, 0)); CHECK(b == false); //on the edge
        b = area2.checkLocationInsideArea(Map2DLocation("maptest", -0.001, -0.001, 0)); CHECK(b); //on the edge
        b = area2.checkLocationInsideArea(Map2DLocation("maptest", 1, 1, 0)); CHECK(b == false); //outside

        b = area3.checkLocationInsideArea(Map2DLocation("maptest", -0.5, -0.5, 0)); CHECK(b); //inside
        b = area3.checkLocationInsideArea(Map2DLocation("maptest", 0, 0, 0)); CHECK(b == false); //outside
        b = area3.checkLocationInsideArea(Map2DLocation("maptest", 0, -0.499, 0)); CHECK(b ==false); //on the vertex??
        b = area3.checkLocationInsideArea(Map2DLocation("maptest", 0, -0.5, 0)); CHECK(b); //on the vertex??
        b = area3.checkLocationInsideArea(Map2DLocation("maptest", 0, -0.501, 0)); CHECK(b); //on the vertex
        b = area3.checkLocationInsideArea(Map2DLocation("maptest", 0, 0.5, 0)); CHECK(b==false); //outside
        b = area3.checkLocationInsideArea(Map2DLocation("maptest", 0.9, 0.5, 0)); CHECK(b); //inside
    }

    SECTION("Checking IMap2D methods")
    {
        PolyDriver ddmapserver;
        PolyDriver ddmapclient;
        IMap2D* imap = nullptr;

        ////////"Checking opening map2DServer and map2DClient polydrivers"
        {
            Property pmapserver_cfg;
            pmapserver_cfg.put("device", "map2DServer");
            //Property& ros_prop = pmapserver_cfg.addGroup("ROS");
            //ros_prop.put("enable_ros_publisher", "0");
            //ros_prop.put("enable_ros_subscriber", "0");
            REQUIRE(ddmapserver.open(pmapserver_cfg)); // ddmapserver open successful

            Property pmapclient_cfg;
            pmapclient_cfg.put("device", "map2DClient");
            pmapclient_cfg.put("local", "/mapClientTest");
            pmapclient_cfg.put("remote", "/mapServer");
            REQUIRE(ddmapclient.open(pmapclient_cfg)); // ddmapclient open successful
            REQUIRE(ddmapclient.view(imap)); // IMap2D interface open successful
        }

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
            yarp::dev::Map2DLocation v;
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
            ret = imap->getArea("area", area);         CHECK(ret==false);
            ret = imap->getArea("area_new", area);      CHECK(ret); 
            ret = imap->getLocation("loc", loc);       CHECK(ret==false);
            ret = imap->getLocation("loc_new", loc);    CHECK(ret);
            
            //final cleanup, already tested        
            ret = imap->clearAllLocations();  CHECK(ret);
            ret = imap->clearAllAreas();  CHECK(ret);
        }

        //////////"Checking IMap2D methods which involve usage of classes MapGrid2D"
        {
            MapGrid2D test_store_map1;
            MapGrid2D test_store_map2;
            MapGrid2D test_get_map;

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

        /////////"Checking map client/server polydrivers closure"
        {
            CHECK(ddmapclient.close());
            CHECK(ddmapserver.close());
            // map2DServer and map2DClient polydrivers closed successfully
        }
    }

    Network::setLocalMode(false);
}
