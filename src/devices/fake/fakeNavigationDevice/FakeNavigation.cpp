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

ReturnValue FakeNavigation::gotoTargetByAbsoluteLocation(Map2DLocation loc)
{
    if (m_status == NavigationStatusEnum::navigation_status_idle)
    {
        m_status = NavigationStatusEnum::navigation_status_moving;
        m_absgoal_loc = loc;
        m_time_counter= this->m_navigation_time;
    }
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::gotoTargetByRelativeLocation(double x, double y, double theta)
{
    yCInfo(FAKENAVIGATION) << "gotoTargetByRelativeLocation not yet implemented";
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::gotoTargetByRelativeLocation(double x, double y)
{
    yCInfo(FAKENAVIGATION) << "gotoTargetByRelativeLocation not yet implemented";
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::followPath(const yarp::dev::Nav2D::Map2DPath& path)
{
    yCInfo(FAKENAVIGATION) << "followPath not yet implemented";
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout)
{
    m_control_out.linear_xvel = x_vel;
    m_control_out.linear_yvel = y_vel;
    m_control_out.angular_vel = theta_vel;
    m_control_out.timeout = timeout;
    m_control_out.reception_time = yarp::os::Time::now();
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::getLastVelocityCommand(double& x_vel, double& y_vel, double& theta_vel)
{
    double current_time = yarp::os::Time::now();
    x_vel = m_control_out.linear_xvel;
    y_vel = m_control_out.linear_yvel;
    theta_vel = m_control_out.angular_vel;
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::stopNavigation()
{
    m_status=NavigationStatusEnum::navigation_status_idle;
    m_absgoal_loc=Map2DLocation();
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::suspendNavigation(double time)
{
    if (m_status == NavigationStatusEnum::navigation_status_moving)
    {
        m_status=NavigationStatusEnum::navigation_status_paused;
    }
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::resumeNavigation()
{
    if (m_status == NavigationStatusEnum::navigation_status_paused)
    {
        m_status = NavigationStatusEnum::navigation_status_moving;
    }
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::getAllNavigationWaypoints(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type, yarp::dev::Nav2D::Map2DPath& waypoints)
{
    yCInfo(FAKENAVIGATION) << "getAllNavigationWaypoints not yet implemented";
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::getCurrentNavigationWaypoint(Map2DLocation& curr_waypoint)
{
    yCInfo(FAKENAVIGATION) << "getCurrentNavigationWaypoint not yet implemented";
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::getCurrentNavigationMap(yarp::dev::Nav2D::NavigationMapTypeEnum map_type, MapGrid2D& map)
{
    yCInfo(FAKENAVIGATION) << "getCurrentNavigationMap not yet implemented";
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::getNavigationStatus(yarp::dev::Nav2D::NavigationStatusEnum& status)
{
    status = m_status;
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::getAbsoluteLocationOfCurrentTarget(Map2DLocation& target)
{
    target = m_absgoal_loc;
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::recomputeCurrentNavigationPath()
{
    if (m_status == NavigationStatusEnum::navigation_status_moving)
    {
        //do something
    }
    return ReturnValue_ok;
}

ReturnValue FakeNavigation::getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta)
{
    yCInfo(FAKENAVIGATION) << "getRelativeLocationOfCurrentTarget not yet implemented";
    return ReturnValue_ok;
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
