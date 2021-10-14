/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yarp_dev_Nav2D_Map2DLocation {
} (
  yarp.name = "yarp::dev::Nav2D::Map2DLocation"
  yarp.includefile = "yarp/dev/Map2DLocation.h"
)

struct yarp_dev_Nav2D_Map2DPath {
} (
  yarp.name = "yarp::dev::Nav2D::Map2DPath"
  yarp.includefile = "yarp/dev/Map2DPath.h"
)

struct yarp_dev_Nav2D_MapGrid2D {
} (
  yarp.name = "yarp::dev::Nav2D::MapGrid2D"
  yarp.includefile = "yarp/dev/MapGrid2D.h"
)

enum yarp_dev_Nav2D_NavigationStatusEnum {
} (
  yarp.name = "yarp::dev::Nav2D::NavigationStatusEnum"
  yarp.includefile = "yarp/dev/INavigation2D.h"
  yarp.enumbase = "yarp::conf::vocab32_t"
)

enum yarp_dev_Nav2D_NavigationMapTypeEnum {
} (
  yarp.name = "yarp::dev::Nav2D::NavigationMapTypeEnum"
  yarp.includefile = "yarp/dev/INavigation2D.h"
  yarp.enumbase = "yarp::conf::vocab32_t"
)

enum yarp_dev_Nav2D_TrajectoryTypeEnum {
} (
  yarp.name = "yarp::dev::Nav2D::TrajectoryTypeEnum"
  yarp.includefile = "yarp/dev/INavigation2D.h"
  yarp.enumbase = "yarp::conf::vocab32_t"
)

// ---------------------------------------------------------------

struct return_get_navigation_status {
  1: bool ret = false;
  2: yarp_dev_Nav2D_NavigationStatusEnum status ( yarp.vocab = "true" );
}

struct return_get_all_nav_waypoints {
  1: bool ret = false;
  2: yarp_dev_Nav2D_Map2DPath waypoints;
}

struct return_get_current_nav_map {
  1: bool ret = false;
  2: yarp_dev_Nav2D_MapGrid2D mapgrid;
}

struct return_get_current_nav_waypoint {
  1: bool ret = false;
  2: yarp_dev_Nav2D_Map2DLocation waypoint;
}

// ---------------------------------------------------------------

struct return_get_abs_loc_of_curr_target {
  1: bool ret = false;
  2: yarp_dev_Nav2D_Map2DLocation loc;
}

struct return_get_rel_loc_of_curr_target {
  1: bool ret = false;
  2: double x;
  3: double y;
  4: double theta;
}

// ---------------------------------------------------------------

struct return_get_last_velocity_command {
  1: bool ret = false;
  2: double x_vel;
  3: double y_vel;
  4: double theta_vel;
}

// ---------------------------------------------------------------

struct return_get_name_of_current_target {
  1: bool ret = false;
  2: string name;
}

// ---------------------------------------------------------------

service INavigation2DMsgs
{
  bool                               stop_navigation_RPC               ();
  bool                               resume_navigation_RPC             ();
  bool                               suspend_navigation_RPC            (1:double time_s);
  bool                               recompute_current_navigation_path_RPC ();
  return_get_navigation_status       get_navigation_status_RPC         ();
  return_get_current_nav_waypoint    get_current_nav_waypoint_RPC      ();
  return_get_all_nav_waypoints       get_all_navigation_waypoints_RPC  (1:yarp_dev_Nav2D_TrajectoryTypeEnum trajectory_type);
  return_get_current_nav_map         get_current_navigation_map_RPC    (1:yarp_dev_Nav2D_NavigationMapTypeEnum map_type);

  bool                               goto_target_by_absolute_location_RPC         (1:yarp_dev_Nav2D_Map2DLocation loc);
  bool                               goto_target_by_relative_location1_RPC        (1:double x, 2:double y);
  bool                               goto_target_by_relative_location2_RPC        (1:double x, 2:double y, 3: double theta);
  return_get_abs_loc_of_curr_target  get_absolute_location_of_current_target_RPC  ();
  return_get_rel_loc_of_curr_target  get_relative_location_of_current_target_RPC  ();
  bool                               goto_target_by_absolute_location_and_set_name_RPC  (1:yarp_dev_Nav2D_Map2DLocation loc, 2: string name);

  bool                               apply_velocity_command_RPC                   (1:double x_vel, 2:double y_vel, 3:double theta_vel, 4:double timeout);
  return_get_last_velocity_command   get_last_velocity_command_RPC                ();

  return_get_name_of_current_target  get_name_of_current_target_RPC               ();
}
