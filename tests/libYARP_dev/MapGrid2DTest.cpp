/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>

#if defined(USE_SYSTEM_CATCH)
#include <catch.hpp>
#else
#include "catch.hpp"
#endif

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

TEST_CASE("dev::MapGrid2DTest", "[yarp::dev]") {

    Network::setLocalMode(true);

    SECTION("Test data type")
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
        CHECK(cell1 == cell1_test); // IMap2D world2Cell() operation successfull

        MapGrid2D test_cnvutils_map2 = test_map;
        yarp::dev::MapGrid2D::XYCell cell2(6, 4);
        yarp::dev::MapGrid2D::XYWorld world2;
        yarp::dev::MapGrid2D::XYWorld world2_test(3, -1);
        world2 = test_cnvutils_map2.cell2World(cell2);
        CHECK(world2 == world2_test); // IMap2D cell2World() operation successfull

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
        // IMap2D isInsideMap() test successfull
    }

    SECTION("Checking standard compliance of description")
    {
        PolyDriver ddmapserver;
        Property pmapserver_cfg;
        pmapserver_cfg.put("device", "map2DServer");
        //Property& ros_prop = pmapserver_cfg.addGroup("ROS");
        //ros_prop.put("enable_ros_publisher", "0");
        //ros_prop.put("enable_ros_subscriber", "0");
        REQUIRE(ddmapserver.open(pmapserver_cfg)); // ddmapserver open reported successfull

        IMap2D* imap = nullptr;
        PolyDriver ddmapclient;
        Property pmapclient_cfg;
        pmapclient_cfg.put("device", "map2DClient");
        pmapclient_cfg.put("local", "/mapClientTest");
        pmapclient_cfg.put("remote", "/mapServer");
        REQUIRE(ddmapclient.open(pmapclient_cfg)); // ddmapclient open reported successfull

        REQUIRE(ddmapclient.view(imap)); // IMap2D interface open reported successfull

        MapGrid2D test_store_map1;
        MapGrid2D test_store_map2;
        MapGrid2D test_get_map;

        test_store_map1.setMapName("test_map1");
        test_store_map2.setMapName("test_map2");
        std::vector<std::string> names;

        imap->store_map(test_store_map1);
        imap->store_map(test_store_map2);
        imap->get_map("test_map1",test_get_map);
        CHECK(test_store_map1.isIdenticalTo(test_get_map)); // IMap2D store/get operation successfull

        imap->get_map_names(names);
        bool b1 = (names.size() == 2);
        bool b2 = false;
        if (b1) {
            b2 = (names[0] == "test_map1" && names[1] == "test_map2");
        }
        CHECK(b1);
        CHECK(b2);
        // IMap2D get_map_names operation successfull")

        imap->remove_map("test_map1");
        imap->get_map_names(names);
        b1 = (names.size() == 1);
        CHECK(b1); // IMap2D remove_map operation successfull

        imap->clearAllMaps();
        imap->get_map_names(names);
        b1 = (names.size() == 0);
        CHECK(b1); // IMap2D clear operation successfull

        //closing the drivers
        CHECK(ddmapclient.close());
        CHECK(ddmapserver.close());
        // map2DServer and map2DClient polydrivers closed succesfully
    }

    Network::setLocalMode(false);
}
