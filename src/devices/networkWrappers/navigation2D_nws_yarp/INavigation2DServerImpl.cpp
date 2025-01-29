/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include "yarp/dev/Map2DPath.h"
#include "INavigation2DServerImpl.h"

/*! \file INavigation2DServerImpl.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace std;

namespace {
YARP_LOG_COMPONENT(NAVIGATION2DSERVER, "yarp.device.navigation2DServer")
}

#define CHECK_POINTER(xxx) {if (xxx==nullptr) {yCError(NAVIGATION2DSERVER, "Invalid interface"); return false;}}

INavigation2DRPCd::INavigation2DRPCd(yarp::dev::Nav2D::INavigation2DTargetActions* iNav_target, yarp::dev::Nav2D::INavigation2DControlActions* iNav_ctrl, yarp::dev::Nav2D::INavigation2DVelocityActions* iNav_vel)
{
    m_iNav_target = iNav_target;
    m_iNav_ctrl = iNav_ctrl;
    m_iNav_vel = iNav_vel;
}

// ------------     INavigation2DControlActions
ReturnValue INavigation2DRPCd::stop_navigation_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iNav_ctrl->stopNavigation();
    if (!ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to stopNavigation");
    }
    m_current_goal_name.clear();
    return ret;
}

ReturnValue INavigation2DRPCd::resume_navigation_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iNav_ctrl->resumeNavigation();
    if (!ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to resumeNavigation");
    }
    return ret;
}

ReturnValue INavigation2DRPCd::suspend_navigation_RPC(double time_s)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iNav_ctrl->suspendNavigation(time_s);
    if (!ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to suspendNavigation");
        return ret;
    }
    return ret;
}

ReturnValue INavigation2DRPCd::recompute_current_navigation_path_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iNav_ctrl->recomputeCurrentNavigationPath();
    if (!ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to recomputeCurrentNavigationPath");
        return ret;
    }
    return ret;
}

return_get_navigation_status       INavigation2DRPCd::get_navigation_status_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    yarp::dev::Nav2D::NavigationStatusEnum status;

    return_get_navigation_status ret;
    ret.ret = m_iNav_ctrl->getNavigationStatus(status);
    if (!ret.ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to getNavigationStatus");
        return ret;
    }
    else
    {
        ret.status = status;
    }
    return ret;
}

return_get_current_nav_waypoint    INavigation2DRPCd::get_current_nav_waypoint_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_current_nav_waypoint ret;

    Map2DLocation loc;
    ret.ret = m_iNav_ctrl->getCurrentNavigationWaypoint(loc);
    if (!ret.ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to getCurrentNavigationWaypoint");
        return ret;
    }
    else
    {
        ret.waypoint = loc;
    }
    return ret;
}

return_get_all_nav_waypoints       INavigation2DRPCd::get_all_navigation_waypoints_RPC(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_all_nav_waypoints ret;

    Map2DPath path;
    ret.ret = m_iNav_ctrl->getAllNavigationWaypoints(trajectory_type, path);
    if (!ret.ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to getAllNavigationWaypoints");
        return ret;
    }
    else
    {
        ret.waypoints = path;
    }
    return ret;
}

return_get_current_nav_map         INavigation2DRPCd::get_current_navigation_map_RPC(yarp::dev::Nav2D::NavigationMapTypeEnum map_type)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_current_nav_map ret;

    MapGrid2D themap;
    ret.ret = m_iNav_ctrl->getCurrentNavigationMap(map_type,themap);
    if (!ret.ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to getRelativeLocationOfCurrentTarget");
        return ret;
    }
    else
    {
        ret.mapgrid = themap;
    }
    return ret;
}



// ------------     INavigation2DTargetActions
ReturnValue INavigation2DRPCd::goto_target_by_absolute_location_RPC(const yarp::dev::Nav2D::Map2DLocation& loc)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iNav_target->gotoTargetByAbsoluteLocation(loc);
    if (!ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to gotoTargetByAbsoluteLocation");
    }
    m_current_goal_name.clear();
    return ret;
}

ReturnValue INavigation2DRPCd::goto_target_by_relative_location1_RPC(double x, double y)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iNav_target->gotoTargetByRelativeLocation(x, y);
    if (!ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to gotoTargetByRelativeLocation");
    }
    m_current_goal_name.clear();
    return ret;
}

ReturnValue INavigation2DRPCd::goto_target_by_relative_location2_RPC(double x, double y, double theta)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iNav_target->gotoTargetByRelativeLocation(x, y, theta);
    if (!ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to gotoTargetByRelativeLocation");
    }
    m_current_goal_name.clear();
    return ret;
}

ReturnValue INavigation2DRPCd::goto_target_by_absolute_location_and_set_name_RPC(const yarp::dev::Nav2D::Map2DLocation& loc, const std::string& name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iNav_target->gotoTargetByAbsoluteLocation(loc);
    if (!ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to gotoTargetByAbsoluteLocation");
    }

    m_current_goal_name.set_current_goal_name(name);
    return ret;
}

ReturnValue INavigation2DRPCd::follow_path_RPC(const yarp::dev::Nav2D::Map2DPath& path)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iNav_target->followPath(path);
    if (!ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to follow path");
    }
    m_current_goal_name.clear();
    return ret;
}

return_get_abs_loc_of_curr_target  INavigation2DRPCd::get_absolute_location_of_current_target_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_abs_loc_of_curr_target ret;

    yarp::dev::Nav2D::Map2DLocation loc;
    ret.ret = m_iNav_target->getAbsoluteLocationOfCurrentTarget(loc);
    if (!ret.ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to getAbsoluteLocationOfCurrentTarget");
        return ret;
    }
    else
    {
        ret.loc = loc;
    }
    return ret;
}

return_get_rel_loc_of_curr_target  INavigation2DRPCd::get_relative_location_of_current_target_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_rel_loc_of_curr_target ret;

    double x,y,t;
    ret.ret = m_iNav_target->getRelativeLocationOfCurrentTarget(x, y, t);
    if (!ret.ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to getRelativeLocationOfCurrentTarget");
        return ret;
    }
    else
    {
        ret.x = x;
        ret.y = y;
        ret.theta = t;
    }
    return ret;
}

// ------------ INavigation2DVelocityActions
ReturnValue INavigation2DRPCd::apply_velocity_command_RPC(double x_vel, double y_vel, double theta_vel, double timeout)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iNav_vel->applyVelocityCommand(x_vel, y_vel, theta_vel);
    if (!ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to applyVelocityCommand");
        return ret;
    }
    return ret;
}

return_get_last_velocity_command   INavigation2DRPCd::get_last_velocity_command_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_last_velocity_command ret;

    double x, y, t;
    ret.ret = m_iNav_vel->getLastVelocityCommand(x, y, t);
    if (!ret.ret)
    {
        yCError(NAVIGATION2DSERVER, "Unable to getLastVelocityCommand");
        return ret;
    }
    else
    {
        ret.x_vel = x;
        ret.y_vel = y;
        ret.theta_vel = t;
    }
    return ret;
}

// ------------ extra
return_get_name_of_current_target  INavigation2DRPCd::get_name_of_current_target_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_name_of_current_target ret;

    bool b = m_current_goal_name.get_current_goal_name(ret.name);
    if (!b)
    {
        yCError(NAVIGATION2DSERVER, "Unable to getNameOfCurrentTarget");
        ret.ret = ReturnValue::return_code::return_value_error_method_failed;
        return ret;
    }

    ret.ret = ReturnValue::return_code::return_value_ok;
    return ret;
}

// ------------  internal stuff
bool LastGoalStorage::set_current_goal_name(const std::string& name)
{
    m_current_goal_name = name;
    return true;
}

bool LastGoalStorage::get_current_goal_name(std::string& name)
{
    if (m_current_goal_name == "")
    {
        return true;
    }
    name = m_current_goal_name;
    return true;
}

bool LastGoalStorage::clear()
{
    m_current_goal_name = "";
    return true;
}
