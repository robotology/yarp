/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_NAVIGATION2DSERVERIMPL_H
#define YARP_DEV_NAVIGATION2DSERVERIMPL_H

#include "INavigation2DMsgs.h"
#include "yarp/dev/Map2DPath.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/ReturnValue.h>

class LastGoalStorage
{
    private:
    std::string m_current_goal_name;

    public:
    bool set_current_goal_name(const std::string& name);
    bool get_current_goal_name(std::string& name);
    bool clear();
};

class INavigation2DRPCd : public INavigation2DMsgs
{
    private:
    yarp::dev::Nav2D::INavigation2DControlActions*   m_iNav_ctrl = nullptr;
    yarp::dev::Nav2D::INavigation2DTargetActions*    m_iNav_target = nullptr;
    yarp::dev::Nav2D::INavigation2DVelocityActions*  m_iNav_vel = nullptr;
    std::mutex                                       m_mutex;

    private:
    LastGoalStorage m_current_goal_name;

    public:
    INavigation2DRPCd (yarp::dev::Nav2D::INavigation2DTargetActions* iNav_target,
                       yarp::dev::Nav2D::INavigation2DControlActions* iNav_ctrl,
                       yarp::dev::Nav2D::INavigation2DVelocityActions*  iNav_vel);
    std::mutex* getMutex() { return &m_mutex; }

    //INavigation2DControlActions
    yarp::dev::ReturnValue             stop_navigation_RPC() override;
    yarp::dev::ReturnValue             resume_navigation_RPC() override;
    yarp::dev::ReturnValue             suspend_navigation_RPC(double time_s) override;
    yarp::dev::ReturnValue             recompute_current_navigation_path_RPC() override;
    return_get_navigation_status       get_navigation_status_RPC() override;
    return_get_current_nav_waypoint    get_current_nav_waypoint_RPC() override;
    return_get_all_nav_waypoints       get_all_navigation_waypoints_RPC(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type) override;
    return_get_current_nav_map         get_current_navigation_map_RPC(yarp::dev::Nav2D::NavigationMapTypeEnum map_type) override;

    //INavigation2DTargetActions
    yarp::dev::ReturnValue             goto_target_by_absolute_location_RPC(const yarp::dev::Nav2D::Map2DLocation& loc) override;
    yarp::dev::ReturnValue             goto_target_by_relative_location1_RPC(double x, double y) override;
    yarp::dev::ReturnValue             goto_target_by_relative_location2_RPC(double x, double y, double theta) override;
    yarp::dev::ReturnValue             follow_path_RPC(const yarp::dev::Nav2D::Map2DPath& path) override;
    return_get_abs_loc_of_curr_target  get_absolute_location_of_current_target_RPC() override;
    return_get_rel_loc_of_curr_target  get_relative_location_of_current_target_RPC() override;
    yarp::dev::ReturnValue             goto_target_by_absolute_location_and_set_name_RPC(const yarp::dev::Nav2D::Map2DLocation& loc, const std::string& name) override;

    //INavigation2DVelocityActions
    yarp::dev::ReturnValue             apply_velocity_command_RPC(double x_vel, double y_vel, double theta_vel,  double timeout) override;
    return_get_last_velocity_command   get_last_velocity_command_RPC() override;

    //extra
    return_get_name_of_current_target  get_name_of_current_target_RPC() override;
};

#endif // YARP_DEV_NAVIGATION2DSERVERIMPL_H
