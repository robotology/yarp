/*
 * Copyright (C) 2016 RobotCub Consortium
 * Authors: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


/**
 *
 * Tests for MapGrid2D
 *
 */

#include <yarp/os/NetType.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/sig/MapGrid2D.h>
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
    virtual ConstString getName() { return "MapGrid2DTest"; }

    void testStandard()
    {
        report(0,"checking standard compliance of description...");

        PolyDriver ddmapserver;
        Property pmapserver_cfg;
        pmapserver_cfg.put("device", "map2DServer");
        //Property& ros_prop = pmapserver_cfg.addGroup("ROS");
        //ros_prop.put("enable_ros_publisher", "0");
        //ros_prop.put("enable_ros_subscriber", "0");
        bool ok_server = ddmapserver.open(pmapserver_cfg);
        checkTrue(ok_server, "ddmapserver open reported successful");

        IMap2D* imap = 0;
        PolyDriver ddmapclient;
        Property pmapclient_cfg;
        pmapclient_cfg.put("device", "map2DClient");
        pmapclient_cfg.put("local", "/mapClientTest");
        pmapclient_cfg.put("remote", "/mapServer");
        bool ok_client = ddmapclient.open(pmapclient_cfg);
        checkTrue(ok_client, "ddmapclient open reported successful");

        bool ok_view = ddmapclient.view(imap);
        checkTrue(ok_view, "IMap2D interface open reported successful");

        MapGrid2D test_store_map1;
        MapGrid2D test_store_map2;
        MapGrid2D test_get_map;

        test_store_map1.m_map_name = "test_map1";
        test_store_map2.m_map_name = "test_map2";
        std::vector<std::string> names;

        imap->store_map(test_store_map1);
        imap->store_map(test_store_map2);
        imap->get_map("test_map1",test_get_map);
        checkTrue(test_store_map1.isIdenticalTo(test_get_map), "IMap2D store/get operation succesfull");
        
        imap->get_map_names(names);
        bool b1 = (names.size()==2);
        bool b2 = (names[0] == "test_map1" && names[1] == "test_map2");
        checkTrue(b1&&b2, "IMap2D get_map_names operation succesfull");

        imap->remove_map("test_map1");
        imap->get_map_names(names);
        b1 = (names.size() == 1);
        checkTrue(b1, "IMap2D remove_map operation succesfull");

        imap->clear();
        imap->get_map_names(names);
        b1 = (names.size() == 0);
        checkTrue(b1, "IMap2D clear operation succesfull");
    }

    virtual void runTests()
    {
        bool netMode = Network::setLocalMode(true);
        testStandard();
    }
};

static MapGrid2DTest theMapGrid2DTest;

UnitTest& getMapGrid2DTest() {
    return theMapGrid2DTest;
}

