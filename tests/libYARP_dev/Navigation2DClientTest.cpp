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

void test_similar_angles(INavigation2D* inav, double angle1, double angle2)
{
    bool b0, b1;
    b0 = inav->setInitialPose(Map2DLocation("map_1", 10.2, 20.1, angle1)); CHECK(b0);
    yarp::os::Time::delay(0.1);
    yInfo() << "Testing angle" << angle1 << " is similar to:" << angle2;
    b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, angle2), 0.1, 10.0); CHECK(b1);
    yInfo() << "Testing angle" << angle1 << " is similar to:" << angle2+5.0;
    b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, angle2 + 5.0), 0.1, 10.0); CHECK(b1);
    yInfo() << "Testing angle" << angle1 << " is different from:" << angle2 + 20.0;
    b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, angle2 + 20.0), 0.1, 10.0); CHECK(!b1);
    yInfo() << "Testing angle" << angle1 << " is similar to:" << angle2-5.0;
    b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, angle2 - 5.0), 0.1, 10.0); CHECK(b1);
    yInfo() << "Testing angle" << angle1 << " is different from:" << angle2 - 20.0;
    b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, angle2 - 20.0), 0.1, 10.0); CHECK(!b1);
}

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
        PolyDriver ddnavclient2;
        INavigation2D* inav = nullptr;
        INavigation2D* inav2 = nullptr;
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
            pnavserver_cfg.put("subdevice", "fakeNavigation");
            REQUIRE(ddnavserver.open(pnavserver_cfg));

            Property pnavclient_cfg;
            pnavclient_cfg.put("device", "navigation2DClient");
            pnavclient_cfg.put("local", "/navigationClientTest");
            pnavclient_cfg.put("navigation_server", "/navigationServer");
            pnavclient_cfg.put("map_locations_server", "/mapServer");
            pnavclient_cfg.put("localization_server", "/localizationServer");
            REQUIRE(ddnavclient.open(pnavclient_cfg));
            REQUIRE(ddnavclient.view(inav));

            Property pnavclient_cfg2;
            pnavclient_cfg2.put("device", "navigation2DClient");
            pnavclient_cfg2.put("local", "/navigationClientTest2");
            pnavclient_cfg2.put("navigation_server", "/navigationServer");
            pnavclient_cfg2.put("map_locations_server", "/mapServer");
            pnavclient_cfg2.put("localization_server", "/localizationServer");
            REQUIRE(ddnavclient2.open(pnavclient_cfg2));
            REQUIRE(ddnavclient2.view(inav2));
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

                //in the following tests, the tolerance is set to 10.0 deg
                test_similar_angles(inav,   +2.0,   +2.0);
                test_similar_angles(inav,   -2.0,   +2.0);
                test_similar_angles(inav,   +2.0,   -2.0);
                test_similar_angles(inav,   -2.0,   -2.0);

                test_similar_angles(inav, +182.0, +182.0);
                test_similar_angles(inav, -182.0, +182.0);
                test_similar_angles(inav, +182.0, -182.0);
                test_similar_angles(inav, -182.0, -182.0);

                test_similar_angles(inav,   2.0,  358.0);
                test_similar_angles(inav,  -2.0,  358.0);
                test_similar_angles(inav,   2.0, -358.0);
                test_similar_angles(inav,  -2.0, -358.0);

                test_similar_angles(inav,  +2.0,  718.0);
                test_similar_angles(inav,  -2.0,  718.0);
                test_similar_angles(inav,  +2.0, -718.0);
                test_similar_angles(inav,  -2.0, -718.0);
            }
            {
                b0 = inav->setInitialPose(my_current_loc); CHECK(b0); yarp::os::Time::delay(0.1);
                yarp::dev::OdometryData my_current_odom;
                b1 = inav->getEstimatedOdometry(my_current_odom); CHECK(b1);
                yInfo() << "Current position is:" << my_current_loc.toString();
                yInfo() << "Estimated Odometry is:"<< my_current_odom.toString();
                bool bodom = fabs(my_current_loc.x- my_current_odom.odom_x) < 0.0000001 &&
                             fabs(my_current_loc.y - my_current_odom.odom_y) < 0.0000001 &&
                             fabs(my_current_loc.theta - my_current_odom.odom_theta) < 0.0000001;
                CHECK(bodom);
            }
        }

        //////////"Checking INavigation2D methods
        {
            bool b0,b1,b2;
            Map2DLocation tloc ("test",1,2,3);
            Map2DLocation gloc1;
            Map2DLocation gloc2;
            Map2DLocation gloc_empty;
            std::string tname ("testLoc");
            std::string gname1;
            std::string gname2;
            NavigationStatusEnum gstat1;
            NavigationStatusEnum gstat2;

            b0 = inav->storeLocation(tname, tloc); CHECK(b0);

            //going to a location by absolute value
            b1 = inav->stopNavigation(); CHECK(b1);
            b1 = inav->getNavigationStatus(gstat1); CHECK(b1); CHECK(gstat1==NavigationStatusEnum::navigation_status_idle);
            b2 = inav2->getNavigationStatus(gstat2);  CHECK(b2); CHECK(gstat2 == NavigationStatusEnum::navigation_status_idle);

            b1 = inav->getAbsoluteLocationOfCurrentTarget(gloc1); CHECK(b1); CHECK(gloc1==gloc_empty);
            b1 = inav->getNameOfCurrentTarget(gname1); CHECK(b1); CHECK(gname1 == "");
            b2 = inav2->getAbsoluteLocationOfCurrentTarget(gloc2); CHECK(b2); CHECK(gloc2 == gloc_empty);
            b2 = inav2->getNameOfCurrentTarget(gname2); CHECK(b2); CHECK(gname2 == "");

            b1 = inav->gotoTargetByAbsoluteLocation(tloc); CHECK(b1);
            b1 = inav->getAbsoluteLocationOfCurrentTarget(gloc1); CHECK(b1); CHECK(gloc1==tloc);
            b1 = inav->getNameOfCurrentTarget(gname1); CHECK(b1); CHECK(gname1 == "");
            b2 = inav2->getAbsoluteLocationOfCurrentTarget(gloc2); CHECK(b2); CHECK(gloc2 == tloc);
            b2 = inav2->getNameOfCurrentTarget(gname2); CHECK(b1); CHECK(gname1 == "");

            //going to an existing location by name
            b1 = inav->stopNavigation(); CHECK(b1);
            b1 = inav->getNavigationStatus(gstat1); CHECK(b1); CHECK(gstat1 == NavigationStatusEnum::navigation_status_idle);
            b2 = inav2->getNavigationStatus(gstat2); CHECK(b2); CHECK(gstat2 == NavigationStatusEnum::navigation_status_idle);

            b1 = inav->getAbsoluteLocationOfCurrentTarget(gloc1); CHECK(b1); CHECK(gloc1 == gloc_empty);
            b1 = inav->getNameOfCurrentTarget(gname1); CHECK(b1); CHECK(gname1 == "");
            b2 = inav2->getAbsoluteLocationOfCurrentTarget(gloc2); CHECK(b2); CHECK(gloc2 == gloc_empty);
            b2 = inav2->getNameOfCurrentTarget(gname2); CHECK(b2); CHECK(gname2 == "");

            b1 = inav->gotoTargetByLocationName(tname); CHECK(b1);
            b1 = inav->getAbsoluteLocationOfCurrentTarget(gloc1); CHECK(b1); CHECK(gloc1 == tloc);
            b1 = inav->getNameOfCurrentTarget(gname1); CHECK(b1); CHECK(gname1 == tname);
            b2 = inav2->getAbsoluteLocationOfCurrentTarget(gloc2); CHECK(b2); CHECK(gloc2 == tloc);
            b2 = inav2->getNameOfCurrentTarget(gname2); CHECK(b2); CHECK(gname2 == tname);

            //trying to goto to a non-existing location by name
            b1 = inav->stopNavigation(); CHECK(b1);
            b1 = inav->getNavigationStatus(gstat1); CHECK(b1); CHECK(gstat1 == NavigationStatusEnum::navigation_status_idle);
            b2 = inav2->getNavigationStatus(gstat2); CHECK(b2); CHECK(gstat2 == NavigationStatusEnum::navigation_status_idle);

            b1 = inav->gotoTargetByLocationName("non-existing-loc"); CHECK(b1==false);
            b1 = inav->getAbsoluteLocationOfCurrentTarget(gloc1); CHECK(b1); CHECK(gloc1 == gloc_empty);
            b1 = inav->getNameOfCurrentTarget(gname1); CHECK(b1); CHECK(gname1 == "");
            b2 = inav2->getAbsoluteLocationOfCurrentTarget(gloc2); CHECK(b2); CHECK(gloc2 == gloc_empty);
            b2 = inav2->getNameOfCurrentTarget(gname2); CHECK(b2); CHECK(gname2 == "");
        }

        //"Close all polydrivers and check"
        {
            CHECK(ddnavclient.close());
            CHECK(ddnavclient2.close());
            CHECK(ddnavserver.close());
            CHECK(ddlocserver.close());
            CHECK(ddmapclient.close());
            CHECK(ddmapserver.close());
        }
    }

    Network::setLocalMode(false);
}
