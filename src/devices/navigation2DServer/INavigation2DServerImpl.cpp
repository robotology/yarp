/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
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

void INavigation2DRPCd::setInterfaces(yarp::dev::Nav2D::INavigation2DTargetActions* iNav_target, yarp::dev::Nav2D::INavigation2DControlActions* iNav_ctrl, yarp::dev::Nav2D::INavigation2DVelocityActions* iNav_vel, yarp::dev::Nav2D::INavigation2DExtraActions* iNav_extra)
{
    m_iNav_target = iNav_target;
    m_iNav_ctrl = iNav_ctrl;
    m_iNav_vel = iNav_vel;
    m_iNav_extra = iNav_extra;
}

// ------------     INavigation2DControlActions
bool INavigation2DRPCd::stop_navigation_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    {if (m_iNav_ctrl == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iNav_ctrl->stopNavigation())
    {
        yCError(NAVIGATION2DSERVER, "Unable to stopNavigation");
        return false;
    }
    return true;
}

bool INavigation2DRPCd::resume_navigation_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    {if (m_iNav_ctrl == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iNav_ctrl->resumeNavigation())
    {
        yCError(NAVIGATION2DSERVER, "Unable to resumeNavigation");
        return false;
    }
    return true;
}

bool INavigation2DRPCd::suspend_navigation_RPC(double time_s)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    {if (m_iNav_ctrl == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iNav_ctrl->suspendNavigation(time_s))
    {
        yCError(NAVIGATION2DSERVER, "Unable to suspendNavigation");
        return false;
    }
    return true;
}

bool INavigation2DRPCd::recompute_current_navigation_path_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    {if (m_iNav_ctrl == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iNav_ctrl->recomputeCurrentNavigationPath())
    {
        yCError(NAVIGATION2DSERVER, "Unable to recomputeCurrentNavigationPath");
        return false;
    }
    return true;
}

return_get_navigation_status       INavigation2DRPCd::get_navigation_status_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_navigation_status ret;

    {if (m_iNav_ctrl == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return ret; }}

    yarp::dev::Nav2D::NavigationStatusEnum status;
    if (!m_iNav_ctrl->getNavigationStatus(status))
    {
        yCError(NAVIGATION2DSERVER, "Unable to getNavigationStatus");
        ret.ret = false;
    }
    else
    {
        ret.ret = true;
        ret.status = status;
    }
    return ret;
}

return_get_current_nav_waypoint    INavigation2DRPCd::get_current_nav_waypoint_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_current_nav_waypoint ret;

    {if (m_iNav_ctrl == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return ret; }}

    Map2DLocation loc;
    if (!m_iNav_ctrl->getCurrentNavigationWaypoint(loc))
    {
        yCError(NAVIGATION2DSERVER, "Unable to getCurrentNavigationWaypoint");
        ret.ret = false;
    }
    else
    {
        ret.ret = true;
        ret.waypoint = loc;
    }
    return ret;
}

return_get_all_nav_waypoints       INavigation2DRPCd::get_all_navigation_waypoints_RPC(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_all_nav_waypoints ret;

    {if (m_iNav_ctrl == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return ret; }}

    Map2DPath path;
    if (!m_iNav_ctrl->getAllNavigationWaypoints(trajectory_type, path))
    {
        yCError(NAVIGATION2DSERVER, "Unable to getAllNavigationWaypoints");
        ret.ret = false;
    }
    else
    {
        ret.ret = true;
        ret.waypoints = path;
    }
    return ret;
}

return_get_current_nav_map         INavigation2DRPCd::get_current_navigation_map_RPC(yarp::dev::Nav2D::NavigationMapTypeEnum map_type)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_current_nav_map ret;

    {if (m_iNav_ctrl == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return ret; }}

    MapGrid2D themap;
    if (!m_iNav_ctrl->getCurrentNavigationMap(map_type,themap))
    {
        yCError(NAVIGATION2DSERVER, "Unable to getRelativeLocationOfCurrentTarget");
        ret.ret = false;
    }
    else
    {
        ret.ret = true;
        ret.mapgrid = themap;
    }
    return ret;
}



// ------------     INavigation2DTargetActions
bool INavigation2DRPCd::goto_target_by_absolute_location_RPC(const yarp::dev::Nav2D::Map2DLocation& loc)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    {if (m_iNav_target == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iNav_target->gotoTargetByAbsoluteLocation(loc))
    {
        yCError(NAVIGATION2DSERVER, "Unable to gotoTargetByAbsoluteLocation");
        return false;
    }
    return true;
}

bool INavigation2DRPCd::goto_target_by_relative_location1_RPC(double x, double y)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    {if (m_iNav_target == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iNav_target->gotoTargetByRelativeLocation(x,y))
    {
        yCError(NAVIGATION2DSERVER, "Unable to gotoTargetByRelativeLocation");
        return false;
    }
    return true;
}

bool INavigation2DRPCd::goto_target_by_relative_location2_RPC(double x, double y, double theta)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    {if (m_iNav_target == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iNav_target->gotoTargetByRelativeLocation(x,y,theta))
    {
        yCError(NAVIGATION2DSERVER, "Unable to gotoTargetByRelativeLocation");
        return false;
    }
    return true;
}

bool INavigation2DRPCd::goto_target_by_absolute_location_and_set_name_RPC(const yarp::dev::Nav2D::Map2DLocation& loc, const std::string& name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    {if (m_iNav_target == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iNav_target->gotoTargetByAbsoluteLocation(loc))
    {
        yCError(NAVIGATION2DSERVER, "Unable to gotoTargetByAbsoluteLocation");
        return false;
    }

    this->set_current_goal_name(name);

    return true;
}

return_get_abs_loc_of_curr_target  INavigation2DRPCd::get_absolute_location_of_current_target_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_abs_loc_of_curr_target ret;

    {if (m_iNav_target == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return ret; }}

    yarp::dev::Nav2D::Map2DLocation loc;
    if (!m_iNav_target->getAbsoluteLocationOfCurrentTarget(loc))
    {
        yCError(NAVIGATION2DSERVER, "Unable to getAbsoluteLocationOfCurrentTarget");
        ret.ret = false;
    }
    else
    {
        ret.ret = true;
        ret.loc = loc;
    }
    return ret;
}

return_get_rel_loc_of_curr_target  INavigation2DRPCd::get_relative_location_of_current_target_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_rel_loc_of_curr_target ret;

    {if (m_iNav_target == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return ret; }}

    double x,y,t;
    if (!m_iNav_target->getRelativeLocationOfCurrentTarget(x,y,t))
    {
        yCError(NAVIGATION2DSERVER, "Unable to getRelativeLocationOfCurrentTarget");
        ret.ret = false;
    }
    else
    {
        ret.ret = true;
        ret.x = x;
        ret.y = y;
        ret.theta = t;
    }
    return ret;
}

// ------------ INavigation2DVelocityActions
bool INavigation2DRPCd::apply_velocity_command_RPC(double x_vel, double y_vel, double theta_vel, double timeout)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    {if (m_iNav_vel == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iNav_vel->applyVelocityCommand(x_vel, y_vel, theta_vel))
    {
        yCError(NAVIGATION2DSERVER, "Unable to applyVelocityCommand");
        return false;
    }
    return true;
}

return_get_last_velocity_command   INavigation2DRPCd::get_last_velocity_command_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_last_velocity_command ret;

    {if (m_iNav_vel == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return ret; }}

    double x, y, t;
    if (!m_iNav_vel->getLastVelocityCommand(x, y, t))
    {
        yCError(NAVIGATION2DSERVER, "Unable to getLastVelocityCommand");
        ret.ret = false;
    }
    else
    {
        ret.ret = true;
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

    {if (m_iNav_extra == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return ret; }}

    std::string name;
    if (!m_iNav_extra->getNameOfCurrentTarget(name))
    {
        yCError(NAVIGATION2DSERVER, "Unable to getNameOfCurrentTarget");
        ret.ret = false;
    }
    else
    {
        ret.ret = true;
        ret.name = name;
    }
    return ret;
}

// ------------  internal stuff
bool INavigation2DRPCd::set_current_goal_name(const std::string& name)
{
    m_current_goal_name = name;
    return true;
}

bool INavigation2DRPCd::get_current_goal_name(std::string& name)
{
    if (m_current_goal_name == "")
    {
        return false;
    }
    name = m_current_goal_name;
    return true;
}

bool INavigation2DRPCd::clear_current_goal_name()
{
    m_current_goal_name = "";
    return true;
}
