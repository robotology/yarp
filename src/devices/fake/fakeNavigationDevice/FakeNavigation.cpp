/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include "FakeNavigation.h"
#include "yarp/dev/Map2DPath.h"
#include <math.h>
#include <cmath>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;

namespace {
YARP_LOG_COMPONENT(FAKENAVIGATION, "yarp.device.fakeNavigation")
}


bool FakeNavigation:: open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) {return false;}

    m_time_counter = m_navigation_time;

    this->start();
    return true;
}

FakeNavigation::FakeNavigation() : yarp::os::PeriodicThread(0.010)
{
}

//module cleanup
bool FakeNavigation:: close()
{
    return true;
}

bool FakeNavigation::gotoTargetByAbsoluteLocation(Map2DLocation loc)
{
    if (m_status == NavigationStatusEnum::navigation_status_idle)
    {
        m_status = NavigationStatusEnum::navigation_status_moving;
        m_absgoal_loc = loc;
        m_time_counter= this->m_navigation_time;
    }
    return true;
}

bool FakeNavigation::gotoTargetByRelativeLocation(double x, double y, double theta)
{
    yCInfo(FAKENAVIGATION) << "gotoTargetByRelativeLocation not yet implemented";
    return true;
}

bool FakeNavigation::gotoTargetByRelativeLocation(double x, double y)
{
    yCInfo(FAKENAVIGATION) << "gotoTargetByRelativeLocation not yet implemented";
    return true;
}

bool followPath(const yarp::dev::Nav2D::Map2DPath& locs)
{
    yCInfo(FAKENAVIGATION) << "followPath not yet implemented";
    return true;
}

bool FakeNavigation::applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout)
{
    m_control_out.linear_xvel = x_vel;
    m_control_out.linear_yvel = y_vel;
    m_control_out.angular_vel = theta_vel;
    m_control_out.timeout = timeout;
    m_control_out.reception_time = yarp::os::Time::now();
    return true;
}

bool FakeNavigation::getLastVelocityCommand(double& x_vel, double& y_vel, double& theta_vel)
{
    double current_time = yarp::os::Time::now();
    x_vel = m_control_out.linear_xvel;
    y_vel = m_control_out.linear_yvel;
    theta_vel = m_control_out.angular_vel;
    return true;
}

bool FakeNavigation::stopNavigation()
{
    m_status=NavigationStatusEnum::navigation_status_idle;
    m_absgoal_loc=Map2DLocation();
    return true;
}

bool FakeNavigation::suspendNavigation(double time)
{
    if (m_status == NavigationStatusEnum::navigation_status_moving)
    {
        m_status=NavigationStatusEnum::navigation_status_paused;
    }
    return true;
}

bool FakeNavigation::resumeNavigation()
{
    if (m_status == NavigationStatusEnum::navigation_status_paused)
    {
        m_status = NavigationStatusEnum::navigation_status_moving;
    }
    return true;
}

bool FakeNavigation::getAllNavigationWaypoints(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type, yarp::dev::Nav2D::Map2DPath& waypoints)
{
    yCInfo(FAKENAVIGATION) << "getAllNavigationWaypoints not yet implemented";
    return true;
}

bool FakeNavigation::getCurrentNavigationWaypoint(Map2DLocation& curr_waypoint)
{
    yCInfo(FAKENAVIGATION) << "getCurrentNavigationWaypoint not yet implemented";
    return true;
}

bool FakeNavigation::getCurrentNavigationMap(yarp::dev::Nav2D::NavigationMapTypeEnum map_type, MapGrid2D& map)
{
    yCInfo(FAKENAVIGATION) << "getCurrentNavigationMap not yet implemented";
    return true;
}

bool FakeNavigation::getNavigationStatus(yarp::dev::Nav2D::NavigationStatusEnum& status)
{
    status = m_status;
    return true;
}

bool FakeNavigation::getAbsoluteLocationOfCurrentTarget(Map2DLocation& target)
{
    target=m_absgoal_loc;
    return true;
}

bool FakeNavigation::recomputeCurrentNavigationPath()
{
    if (m_status == NavigationStatusEnum::navigation_status_moving)
    {
        //do something
    }
    return true;
}

bool FakeNavigation::getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta)
{
    yCInfo(FAKENAVIGATION) << "getRelativeLocationOfCurrentTarget not yet implemented";
    return true;
}

bool FakeNavigation::threadInit()
{
    return true;
}

void FakeNavigation::threadRelease()
{

}

void FakeNavigation::run()
{
    if (m_status == NavigationStatusEnum::navigation_status_moving)
    {
        if (m_time_counter>0)
        {
            m_time_counter--;
        }
        else
        {
            m_status = NavigationStatusEnum::navigation_status_goal_reached;
            m_time_counter = m_reached_time;
        }
    }
    if (m_status == NavigationStatusEnum::navigation_status_goal_reached)
    {
        if (m_time_counter > 0)
        {
            m_time_counter--;
        }
        else
        {
            m_status = NavigationStatusEnum::navigation_status_idle;
        }
    }
}
