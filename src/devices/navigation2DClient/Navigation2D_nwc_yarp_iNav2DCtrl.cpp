/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Navigation2D_nwc_yarp.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cmath>

/*! \file Navigation2D_nwc_yarp_iNav2DCtrl.cpp */

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(NAVIGATION2D_NWC_YARP, "yarp.device.navigation2D_nwc_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

bool Navigation2D_nwc_yarp::getNavigationStatus(NavigationStatusEnum& status)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_nav_RPC.get_navigation_status_RPC();
    if (!ret.ret)
    {
        yCError(NAVIGATION2D_NWC_YARP, "Unable to get_navigation_status_RPC");
        return false;
    }
    status = yarp::dev::Nav2D::NavigationStatusEnum(ret.status);
    return true;
}

bool  Navigation2D_nwc_yarp::recomputeCurrentNavigationPath()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_nav_RPC.recompute_current_navigation_path_RPC();
}

bool Navigation2D_nwc_yarp::suspendNavigation(const double time_s)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_nav_RPC.suspend_navigation_RPC(time_s);
}

bool Navigation2D_nwc_yarp::stopNavigation()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_nav_RPC.stop_navigation_RPC();
}

bool Navigation2D_nwc_yarp::resumeNavigation()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_nav_RPC.resume_navigation_RPC();
}

bool   Navigation2D_nwc_yarp::getAllNavigationWaypoints(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type, yarp::dev::Nav2D::Map2DPath& waypoints)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_nav_RPC.get_all_navigation_waypoints_RPC(trajectory_type);
    if (!ret.ret)
    {
        yCError(NAVIGATION2D_NWC_YARP, "Unable to get_current_nav_waypoint_RPC");
        return false;
    }
    waypoints = ret.waypoints;
    return true;
}

bool   Navigation2D_nwc_yarp::getCurrentNavigationWaypoint(Map2DLocation& curr_waypoint)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_nav_RPC.get_current_nav_waypoint_RPC();
    if (!ret.ret)
    {
        yCError(NAVIGATION2D_NWC_YARP, "Unable to get_current_nav_waypoint_RPC");
        return false;
    }
    curr_waypoint = ret.waypoint;
    return true;
}

bool Navigation2D_nwc_yarp::getCurrentNavigationMap(yarp::dev::Nav2D::NavigationMapTypeEnum map_type,MapGrid2D& map)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_nav_RPC.get_current_navigation_map_RPC(map_type);
    if (!ret.ret)
    {
        yCError(NAVIGATION2D_NWC_YARP, "Unable to get_current_nav_waypoint_RPC");
        return false;
    }
    map = ret.mapgrid;
    return true;
}
