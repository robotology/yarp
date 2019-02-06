/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

/**
 *
 * Tests for MapGrid2D
 *
 */

#include <yarp/os/NetType.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>

#include "TestList.h"

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace yarp::os::impl;

class MapGrid2DTest : public UnitTest {
public:
    virtual std::string getName() const override { return "MapGrid2DTest"; }

    void ReadMapfromString(MapGrid2D& m, std::string s)
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

    bool testDataType()
    {
        MapGrid2D test_map;
        test_map.setResolution(1.0);
        test_map.setSize_in_meters(11, 11);
        test_map.setOrigin(-3, -7, 0);

        //This is the test map
        /////1 3 5 7 9 
        ////0 2 4 6 8 0
        std::string mapstring("***********\n"\
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
        checkTrue(cell1 == cell1_test, "IMap2D world2Cell() operation successfull");

        MapGrid2D test_cnvutils_map2 = test_map;
        yarp::dev::MapGrid2D::XYCell cell2(6, 4);
        yarp::dev::MapGrid2D::XYWorld world2;
        yarp::dev::MapGrid2D::XYWorld world2_test(3, -1);
        world2 = test_cnvutils_map2.cell2World(cell2);
        checkTrue(world2 == world2_test, "IMap2D cell2World() operation successfull");

        MapGrid2D test_cnvutils_map3 = test_map;
        yarp::dev::MapGrid2D::XYCell cell3_ok(0, 0);
        yarp::dev::MapGrid2D::XYWorld world3_ok(-1, -1);
        yarp::dev::MapGrid2D::XYCell cell4_err(11, 11);
        yarp::dev::MapGrid2D::XYWorld world4_err(100, 100);
        yarp::dev::MapGrid2D::XYCell cell5_err(-1, -1);
        yarp::dev::MapGrid2D::XYWorld world5_err(-100, -100);
        checkTrue(test_cnvutils_map3.isInsideMap(cell3_ok) == true &&
            test_cnvutils_map3.isInsideMap(world3_ok) == true &&
            test_cnvutils_map3.isInsideMap(cell4_err) == false &&
            test_cnvutils_map3.isInsideMap(world4_err) == false &&
            test_cnvutils_map3.isInsideMap(cell5_err) == false &&
            test_cnvutils_map3.isInsideMap(world5_err) == false, "IMap2D isInsideMap() test successfull");
        return true;
    }

    bool testClientServer()
    {
        report(0,"checking standard compliance of description...");

        PolyDriver ddmapserver;
        Property pmapserver_cfg;
        pmapserver_cfg.put("device", "map2DServer");
        //Property& ros_prop = pmapserver_cfg.addGroup("ROS");
        //ros_prop.put("enable_ros_publisher", "0");
        //ros_prop.put("enable_ros_subscriber", "0");
        bool ok_server = ddmapserver.open(pmapserver_cfg);
        checkTrue(ok_server, "ddmapserver open reported successfull");

        IMap2D* imap = nullptr;
        PolyDriver ddmapclient;
        Property pmapclient_cfg;
        pmapclient_cfg.put("device", "map2DClient");
        pmapclient_cfg.put("local", "/mapClientTest");
        pmapclient_cfg.put("remote", "/mapServer");
        bool ok_client = ddmapclient.open(pmapclient_cfg);
        checkTrue(ok_client, "ddmapclient open reported successfull");

        bool ok_view = ddmapclient.view(imap);
        checkTrue(ok_view, "IMap2D interface open reported successfull");

        if (ok_client == false || ok_view == false)
        {
            //test cannot continue
            return false;
        }

        MapGrid2D test_store_map1;
        MapGrid2D test_store_map2;
        MapGrid2D test_get_map;

        test_store_map1.setMapName("test_map1");
        test_store_map2.setMapName("test_map2");
        std::vector<std::string> names;

        imap->store_map(test_store_map1);
        imap->store_map(test_store_map2);
        imap->get_map("test_map1",test_get_map);
        checkTrue(test_store_map1.isIdenticalTo(test_get_map), "IMap2D store/get operation successfull");
        
        imap->get_map_names(names);
        bool b1 = (names.size()==2);
        bool b2 = false;
        if (b1)
        {
            b2 = (names[0] == "test_map1" && names[1] == "test_map2");
        }
        checkTrue(b1&&b2, "IMap2D get_map_names operation successfull");

        imap->remove_map("test_map1");
        imap->get_map_names(names);
        b1 = (names.size() == 1);
        checkTrue(b1, "IMap2D remove_map operation successfull");

        imap->clear();
        imap->get_map_names(names);
        b1 = (names.size() == 0);
        checkTrue(b1, "IMap2D clear operation successfull");

        //closing the drivers
        bool bclose1 = ddmapclient.close();
        bool bclose2 = ddmapserver.close();
        checkTrue(bclose1 && bclose2, "map2DServer and map2DClient polydrivers closed succesfully");

        return true;
    }

    virtual void runTests() override
    {
        Network::setLocalMode(true);
        testDataType();
        testClientServer();
        Network::setLocalMode(false);
    }
};

static MapGrid2DTest theMapGrid2DTest;

UnitTest& getMapGrid2DTest() {
    return theMapGrid2DTest;
}

