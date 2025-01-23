/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef INAVIGATION2DTEST_H
#define INAVIGATION2DTEST_H

#include <yarp/dev/IMap2D.h>
#include <yarp/dev/INavigation2D.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void test_similar_angles(INavigation2D* inav, double angle1, double angle2)
    {
        bool b0, b1;
        b0 = inav->setInitialPose(Map2DLocation("map_1", 10.2, 20.1, angle1)); CHECK(b0);
        yarp::os::Time::delay(0.1);
        INFO("Testing angle" + std::to_string(angle1) + " is similar to:" + std::to_string(angle2));
        bool is_near;
        b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, angle2), is_near, 0.1, 10.0);   CHECK(b1); CHECK(is_near);
        //yInfo() << "Testing angle" << angle1 << " is similar to:" << angle2 + 5.0;
        b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, angle2 + 5.0), is_near,0.1, 10.0); CHECK(b1); CHECK(is_near);
        //yInfo() << "Testing angle" << angle1 << " is different from:" << angle2 + 20.0;
        b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, angle2 + 20.0), is_near,0.1, 10.0); CHECK(b1);  CHECK(!is_near);
        //yInfo() << "Testing angle" << angle1 << " is similar to:" << angle2 - 5.0;
        b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, angle2 - 5.0), is_near,0.1, 10.0); CHECK(b1); CHECK(is_near);
        //yInfo() << "Testing angle" << angle1 << " is different from:" << angle2 - 20.0;
        b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, angle2 - 20.0), is_near,0.1, 10.0); CHECK(b1); CHECK(!is_near);
    }

    inline void exec_iNav2D_test_1(INavigation2D* inav, IMap2D* imap)
    {
        //////////"Checking INavigation2D methods
        Map2DLocation loc_test = Map2DLocation("map_1", 10.0, 20.0, 15);
        Map2DLocation my_current_loc = Map2DLocation("map_1", 10.2, 20.1, 15.5);
        Map2DLocation loc_to_be_tested;
        Map2DArea area_test("map_1", std::vector<Map2DLocation> {Map2DLocation("map_1", -10, -10, 0),
            Map2DLocation("map_1", -10, +10, 0),
            Map2DLocation("map_1", +10, +10, 0),
            Map2DLocation("map_1", +10, -10, 0)}, "this is a test area");
        bool b0, b1;
        b0 = imap->storeArea("area_test", area_test); CHECK(b0);
        b0 = imap->storeLocation("loc_test", loc_test); CHECK(b0);

        {
            bool is_inside;
            b0 = inav->setInitialPose(my_current_loc); CHECK(b0);
            yarp::os::Time::delay(0.1);
            b0 = inav->getCurrentPosition(loc_to_be_tested); CHECK(b0); CHECK(loc_to_be_tested == my_current_loc);
            b1 = inav->checkInsideArea("area_test",is_inside);  CHECK(b1); CHECK(!is_inside);
            b1 = inav->checkInsideArea(area_test,is_inside);    CHECK(b1); CHECK(!is_inside);
            b0 = inav->setInitialPose(Map2DLocation("map_1", 0, 0, 0)); CHECK(b0);
            yarp::os::Time::delay(0.1);
            b0 = inav->getCurrentPosition(loc_to_be_tested); CHECK(loc_to_be_tested == Map2DLocation("map_1", 0, 0, 0));
            b1 = inav->checkInsideArea("area_test",is_inside);  CHECK(b1); CHECK(is_inside);
            b1 = inav->checkInsideArea(area_test,is_inside);    CHECK(b1); CHECK(is_inside);
        }

        {
            double lin_tol = 1.0; //m
            double ang_tol = 1.0; //deg
            b0 = inav->setInitialPose(my_current_loc); CHECK(b0);
            yarp::os::Time::delay(0.1);
            bool is_near;
            b1 = inav->checkNearToLocation("loc_test", is_near, lin_tol, ang_tol); CHECK(b1); CHECK(is_near);
            b1 = inav->checkNearToLocation(loc_test, is_near, lin_tol, ang_tol); CHECK(b1); CHECK(is_near);
        }
        {
            double lin_tol = 0.0001; //m
            double ang_tol = 0.0001; //deg
            b0 = inav->setInitialPose(my_current_loc); CHECK(b0);
            yarp::os::Time::delay(0.1);
            bool is_near;
            b1 = inav->checkNearToLocation("loc_test", is_near, lin_tol, ang_tol); CHECK(b1); CHECK(!is_near);
            b1 = inav->checkNearToLocation(loc_test, is_near, lin_tol, ang_tol); CHECK(b1); CHECK(!is_near);
        }
        {
            double lin_tol = 1.0; //m
            Map2DLocation my_current_loc2 = my_current_loc; my_current_loc2.theta = 90;
            b0 = inav->setInitialPose(my_current_loc2); CHECK(b0);
            yarp::os::Time::delay(0.1);
            bool is_near;
            b1 = inav->checkNearToLocation("loc_test", is_near, lin_tol); CHECK(b1); CHECK(is_near);
            b1 = inav->checkNearToLocation(loc_test, is_near, lin_tol); CHECK(b1); CHECK(is_near);
        }
        {
            double lin_tol = 0.1; //m
            double ang_tol = 0.1; //deg
            bool is_near;
            b0 = inav->setInitialPose(my_current_loc); CHECK(b0); yarp::os::Time::delay(0.1);
            b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5), is_near, lin_tol, ang_tol); CHECK(b1);
            b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5 + 180), is_near, lin_tol, ang_tol); CHECK(b1); CHECK(!is_near);
            b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5 + 360), is_near, lin_tol, ang_tol); CHECK(b1); CHECK(is_near);
            b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5 + 720), is_near, lin_tol, ang_tol); CHECK(b1); CHECK(is_near);
            b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5 - 180), is_near, lin_tol, ang_tol); CHECK(b1); CHECK(!is_near);
            b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5 - 360), is_near, lin_tol, ang_tol); CHECK(b1); CHECK(is_near);
            b1 = inav->checkNearToLocation(Map2DLocation("map_1", 10.2, 20.1, 15.5 - 720), is_near, lin_tol, ang_tol); CHECK(b1); CHECK(is_near);

            //in the following tests, the tolerance is set to 10.0 deg
            test_similar_angles(inav, +2.0, +2.0);
            test_similar_angles(inav, -2.0, +2.0);
            test_similar_angles(inav, +2.0, -2.0);
            test_similar_angles(inav, -2.0, -2.0);

            test_similar_angles(inav, +182.0, +182.0);
            test_similar_angles(inav, -182.0, +182.0);
            test_similar_angles(inav, +182.0, -182.0);
            test_similar_angles(inav, -182.0, -182.0);

            test_similar_angles(inav, 2.0, 358.0);
            test_similar_angles(inav, -2.0, 358.0);
            test_similar_angles(inav, 2.0, -358.0);
            test_similar_angles(inav, -2.0, -358.0);

            test_similar_angles(inav, +2.0, 718.0);
            test_similar_angles(inav, -2.0, 718.0);
            test_similar_angles(inav, +2.0, -718.0);
            test_similar_angles(inav, -2.0, -718.0);
        }
        {
            b0 = inav->setInitialPose(my_current_loc); CHECK(b0); yarp::os::Time::delay(0.1);
            yarp::dev::OdometryData my_current_odom;
            b1 = inav->getEstimatedOdometry(my_current_odom); CHECK(b1);
            INFO("Current position is:" + my_current_loc.toString());
            INFO("Estimated Odometry is:" + my_current_odom.toString());
            bool bodom = fabs(my_current_loc.x - my_current_odom.odom_x) < 0.0000001 &&
                fabs(my_current_loc.y - my_current_odom.odom_y) < 0.0000001 &&
                fabs(my_current_loc.theta - my_current_odom.odom_theta) < 0.0000001;
            CHECK(bodom);
        }
    }

    inline void exec_iNav2D_test_2(INavigation2D* inav, IMap2D* imap)
    {
        //////////"Checking INavigation2D methods
        bool b0, b1, b2;
        Map2DLocation tloc("test", 1, 2, 3);
        Map2DLocation gloc1;
        Map2DLocation gloc_empty;
        std::string tname("testLoc");
        std::string gname1;
        NavigationStatusEnum gstat1;

        b0 = inav->storeLocation(tname, tloc); CHECK(b0);

        //going to a location by absolute value
        b1 = inav->stopNavigation(); CHECK(b1);
        b1 = inav->getNavigationStatus(gstat1); CHECK(b1); CHECK(gstat1 == NavigationStatusEnum::navigation_status_idle);

        b1 = inav->getAbsoluteLocationOfCurrentTarget(gloc1); CHECK(b1); CHECK(gloc1 == gloc_empty);
        b1 = inav->getNameOfCurrentTarget(gname1); CHECK(b1); CHECK(gname1 == "");
        b1 = inav->gotoTargetByAbsoluteLocation(tloc); CHECK(b1);
        b1 = inav->getAbsoluteLocationOfCurrentTarget(gloc1); CHECK(b1); CHECK(gloc1 == tloc);
        b1 = inav->getNameOfCurrentTarget(gname1); CHECK(b1); CHECK(gname1 == "");

        //going to an existing location by name
        b1 = inav->stopNavigation(); CHECK(b1);
        b1 = inav->getNavigationStatus(gstat1); CHECK(b1); CHECK(gstat1 == NavigationStatusEnum::navigation_status_idle);

        b1 = inav->getAbsoluteLocationOfCurrentTarget(gloc1); CHECK(b1); CHECK(gloc1 == gloc_empty);
        b1 = inav->getNameOfCurrentTarget(gname1); CHECK(b1); CHECK(gname1 == "");

        b1 = inav->gotoTargetByLocationName(tname); CHECK(b1);
        b1 = inav->getAbsoluteLocationOfCurrentTarget(gloc1); CHECK(b1); CHECK(gloc1 == tloc);
        b1 = inav->getNameOfCurrentTarget(gname1); CHECK(b1); CHECK(gname1 == tname);

        //stop must clear navigation target name
        b1 = inav->stopNavigation(); CHECK(b1);
        b1 = inav->getNavigationStatus(gstat1); CHECK(b1); CHECK(gstat1 == NavigationStatusEnum::navigation_status_idle);
        b1 = inav->getNameOfCurrentTarget(gname1); CHECK(b1); CHECK(gname1 == "");

        //trying to goto to a non-existing location by name, target name must be not set
        b1 = inav->gotoTargetByLocationName("non-existing-loc"); CHECK(b1 == false);
        b1 = inav->getAbsoluteLocationOfCurrentTarget(gloc1); CHECK(b1); CHECK(gloc1 == gloc_empty);
        b1 = inav->getNameOfCurrentTarget(gname1); CHECK(b1); CHECK(gname1 == "");

        //mix of last two tests. A non existing location must clear a previously set target name
        b1 = inav->gotoTargetByLocationName(tname); CHECK(b1);
        b1 = inav->getAbsoluteLocationOfCurrentTarget(gloc1); CHECK(b1); CHECK(gloc1 == tloc);
        b1 = inav->gotoTargetByLocationName("non-existing-loc"); CHECK(b1 == false);
        b1 = inav->getAbsoluteLocationOfCurrentTarget(gloc1); CHECK(b1); CHECK(gloc1 == gloc_empty);
    }

    inline void exec_iNav2D_test_3(INavigation2DTargetActions* inav_trgt, INavigation2DControlActions* inav_ctl)
    {
        //////////"Checking INavigation2D methods
        bool b;
        Map2DLocation loc("test", 1, 2, 3);
        yarp::dev::Nav2D::NavigationStatusEnum status;

        b = inav_ctl->getNavigationStatus(status); CHECK(b);
        CHECK(status== yarp::dev::Nav2D::NavigationStatusEnum::navigation_status_idle);

        b = inav_trgt->gotoTargetByAbsoluteLocation(loc); CHECK(b);
        b = inav_ctl->getNavigationStatus(status); CHECK(b);
        CHECK(status == yarp::dev::Nav2D::NavigationStatusEnum::navigation_status_moving);

        size_t count=0;
        do
        {
            b = inav_ctl->getNavigationStatus(status); CHECK(b);
            if (status == yarp::dev::Nav2D::NavigationStatusEnum::navigation_status_goal_reached)
            {
                break;
            }
            yarp::os::Time::delay(0.1);
            count++;
            if (count>200) {CHECK(0); break; }
        }
        while(1);

        count = 0;
        do
        {
            b = inav_ctl->getNavigationStatus(status); CHECK(b);
            if (status == yarp::dev::Nav2D::NavigationStatusEnum::navigation_status_idle)
            {
                break;
            }
            yarp::os::Time::delay(0.1);
            count++;
            if (count > 200) { CHECK(0); break; }
        } while (1);

        //test complete
    }
}

#endif
