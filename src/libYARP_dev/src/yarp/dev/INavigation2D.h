/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_INAVIGATION2D_H
#define YARP_DEV_INAVIGATION2D_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/ILocalization2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DPath.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/dev/MapGrid2D.h>
#include <vector>
#include <limits>
#include <string>

namespace yarp::dev::Nav2D {
class INavigation2DVelocityActions;
class INavigation2DTargetActions;
class INavigation2DControlActions;
class INavigation2DExtraActions;
class INavigation2D;

enum NavigationStatusEnum
{
    navigation_status_idle = yarp::os::createVocab32('i', 'd', 'l', 'e'),
    navigation_status_preparing_before_move = yarp::os::createVocab32('p', 'r', 'e', 'p'),
    navigation_status_moving = yarp::os::createVocab32('m', 'o', 'v', 'g'),
    navigation_status_waiting_obstacle = yarp::os::createVocab32('w', 'a', 'i', 't'),
    navigation_status_goal_reached = yarp::os::createVocab32('r', 'e', 'c', 'h'),
    navigation_status_aborted = yarp::os::createVocab32('a', 'b', 'r', 't'),
    navigation_status_failing = yarp::os::createVocab32('f', 'a', 'i', 'l'),
    navigation_status_paused = yarp::os::createVocab32('p', 'a', 'u', 's'),
    navigation_status_thinking = yarp::os::createVocab32('t', 'h', 'n', 'k'),
    navigation_status_error = yarp::os::createVocab32('e', 'r', 'r')
};

enum NavigationMapTypeEnum
{
    global_map = yarp::os::createVocab32('g', 'l', 'o', 'b'),
    local_map = yarp::os::createVocab32('l', 'o', 'c', 'a')
};

enum TrajectoryTypeEnum
{
    global_trajectory = yarp::os::createVocab32('g', 'l', 'o', 'b'),
    local_trajectory = yarp::os::createVocab32('l', 'o', 'c', 'a')
};

namespace INavigation2DHelpers
{
    //converts a string to a NavigationStatusEnum.
    //navigation_status_error is returned if the string is not recognized.
    YARP_dev_API NavigationStatusEnum stringToStatus(std::string s);

    //converts a NavigationStatusEnum to a string.
    YARP_dev_API std::string statusToString(NavigationStatusEnum status);
} // namespace INavigation2DHelpers
} // namespace yarp::dev::Nav2D

class YARP_dev_API yarp::dev::Nav2D::INavigation2DVelocityActions
{
public:
    /**
     * Destructor.
     */
    virtual ~INavigation2DVelocityActions() {}

    /**
     * Apply a velocity command. Velocities are expressed in the robot reference frame.
     * @param x [m/s]
     * @param y [m/s]
     * @param theta [deg/s]
     * @param timeout The velocity command expires after the specified amount of time (by default 0.1 seconds)
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout = 0.1) = 0;

    /**
     * Returns the last applied velocity command.
     * @param x [m/s]
     * @param y [m/s]
     * @param theta [deg/s]
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue getLastVelocityCommand(double& x_vel, double& y_vel, double& theta_vel) = 0;
};

class YARP_dev_API yarp::dev::Nav2D::INavigation2DTargetActions
{
public:
    /**
     * Destructor.
     */
    virtual ~INavigation2DTargetActions() {}

    /**
     * Ask the robot to reach a position defined in the world reference frame
     * @param loc the location to be reached
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue gotoTargetByAbsoluteLocation(yarp::dev::Nav2D::Map2DLocation loc) = 0;

    /**
     * Ask the robot to navigate through a set of locations defined in the world reference frame
     * @param path the locations to be reached
     * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue followPath(const yarp::dev::Nav2D::Map2DPath& path) = 0;

    /**
     * Gets the last navigation target in the world reference frame
     * @param loc the location of the robot
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue getAbsoluteLocationOfCurrentTarget(yarp::dev::Nav2D::Map2DLocation& loc) = 0;

    /**
     * Ask the robot to reach a position defined in the robot reference frame. The final orientation of the goal is unspecified.
     * @param x
     * @param y
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue gotoTargetByRelativeLocation(double x, double y) = 0;

    /**
     * Ask the robot to reach a position defined in the robot reference frame
     * @param x
     * @param y
     * @param theta
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue gotoTargetByRelativeLocation(double x, double y, double theta) = 0;

    /**
     * Gets the last navigation target in the robot reference frame
     * @param x
     * @param y
     * @param theta
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta) = 0;
};

class YARP_dev_API yarp::dev::Nav2D::INavigation2DControlActions
{
public:
    /**
     * Destructor.
     */
    virtual ~INavigation2DControlActions() {}

    /**
     * Gets the current status of the navigation task.
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue getNavigationStatus(NavigationStatusEnum& status) = 0;

    /**
     * Terminates the current navigation task. Cannot be resumed.
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue stopNavigation() = 0;

    /**
     * Ask to the robot to suspend the current navigation task for a defined amount of time. Can be resumed by resume().
     * @param time_s
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue suspendNavigation(const double time_s= std::numeric_limits<double>::infinity()) = 0;

    /**
     * Resume a previously suspended navigation task.
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue resumeNavigation() = 0;

    /**
     * Forces the navigation system to recompute the path from the current robot position to the current goal.
     * If no goal has been set, the command has no effect.
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue recomputeCurrentNavigationPath() = 0;

    /**
     * Returns the list of waypoints generated by the navigation algorithm
     * @param trajectory_type specifies if we are requesting the waypoints of the global trajectory or the waypoints of the local trajectory
     * @param waypoints the list of waypoints generated by the navigation algorithm
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue getAllNavigationWaypoints(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type, yarp::dev::Nav2D::Map2DPath& waypoints) = 0;

    /**
     * Returns the current waypoint pursued by the navigation algorithm
     * @param curr_waypoint the current waypoint pursued by the navigation algorithm
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue getCurrentNavigationWaypoint(yarp::dev::Nav2D::Map2DLocation& curr_waypoint) = 0;

    /**
     * Returns the current navigation map processed by the navigation algorithm
     * @param map_type the map to be requested (e.g. global, local, etc.)
     * @param map the map, currently used by the navigation algorithm
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue getCurrentNavigationMap(yarp::dev::Nav2D::NavigationMapTypeEnum map_type, yarp::dev::Nav2D::MapGrid2D& map) = 0;
};

class YARP_dev_API yarp::dev::Nav2D::INavigation2DExtraActions
{
public:
    /**
     * Ask the robot to reach a previously stored location/area
     * @param location_name the name of a location/area previously saved
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue gotoTargetByLocationName(std::string location_or_area_name) = 0;

    /**
     * Check if the robot is currently inside the specified area
     * @param area_name the name of an area previously saved
     * @return is_inside true if the robot is inside the area.
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue checkInsideArea(std::string area_name, bool& is_inside) = 0;

    /**
     * Check if the robot is currently inside the specified area
     * @param area the area to be checked
     * @return is_inside true if the robot is inside the area.
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue checkInsideArea(Nav2D::Map2DArea area, bool& is_inside) = 0;

    /**
     * Check if the robot is inside any of the currently stored areas. If yes, it returns its data.
     * @return area_name name of the area (if found), empty string otherwise
     * @return area data of the area (if found), empty data otherwise
     */
    virtual yarp::dev::ReturnValue inWhichAreaIAm(std::string& area_name, Nav2D::Map2DArea& area) = 0;

    /**
     * Check if the robot is currently near to the specified area
     * @param loc the location to be checked
     * @return is_near true if the robot is near to the area.
     * @param linear_tolerance linear tolerance [m]
     * @param angular_tolerance [deg 0-360]
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue checkNearToLocation(Nav2D::Map2DLocation loc, bool& is_near, double linear_tolerance, double angular_tolerance = std::numeric_limits<double>::infinity()) = 0;

    /**
     * Check if the robot is currently near to the specified area
     * @param location_name the name of the location: it will be searched in the server
     * @return is_near true if the robot is near to the area.
     * @param linear_tolerance linear tolerance [m]
     * @param angular_tolerance [deg]
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue checkNearToLocation(std::string location_name, bool& is_near, double linear_tolerance, double angular_tolerance = std::numeric_limits<double>::infinity()) = 0;

    /**
     * Gets the name of the current target, if available (set by gotoTargetByLocationName)
     * @param location_name the name of the current target
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue getNameOfCurrentTarget(std::string& location_name) = 0;

    /**
     * Store the current location of the robot
     * @param location_name the name of the location
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue storeCurrentPosition(std::string location_name) = 0;

    /**
     * Destructor.
     */
    virtual ~INavigation2DExtraActions() {}
};

/**
 * @ingroup dev_iface_navigation
 *
 * An interface to control the navigation of a mobile robot in a 2D environment.
 */
class YARP_dev_API yarp::dev::Nav2D::INavigation2D :  public yarp::dev::Nav2D::INavigation2DTargetActions,
                                                      public yarp::dev::Nav2D::INavigation2DControlActions,
                                                      public yarp::dev::Nav2D::INavigation2DVelocityActions,
                                                      public yarp::dev::Nav2D::INavigation2DExtraActions,
                                                      public yarp::dev::Nav2D::IMap2D,
                                                      public yarp::dev::Nav2D::ILocalization2D
{
public:
    /**
     * Destructor.
     */
    virtual ~INavigation2D() {}
};

constexpr yarp::conf::vocab32_t VOCAB_NAV_STOP                     = yarp::os::createVocab32('s', 't', 'o', 'p');
constexpr yarp::conf::vocab32_t VOCAB_NAV_SUSPEND                  = yarp::os::createVocab32('s', 'u', 's', 'p');
constexpr yarp::conf::vocab32_t VOCAB_NAV_RESUME                   = yarp::os::createVocab32('r', 'e', 's', 'm');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_NAVIGATION_WAYPOINTS = yarp::os::createVocab32('w', 'a', 'y', 's');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_CURRENT_WAYPOINT     = yarp::os::createVocab32('w', 'a', 'y');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_NAV_MAP              = yarp::os::createVocab32('n', 'm', 'a', 'p');

#endif // YARP_DEV_INAVIGATION2D_H
