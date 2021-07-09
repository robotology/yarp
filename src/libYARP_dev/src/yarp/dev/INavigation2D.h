/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_INAVIGATION2D_H
#define YARP_DEV_INAVIGATION2D_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ILocalization2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DPath.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/dev/MapGrid2D.h>
#include <vector>
#include <limits>
#include <string>

namespace yarp {
    namespace dev {
        namespace Nav2D
        {
            class INavigation2DTargetActions;
            class INavigation2DControlActions;
            class INavigation2D;

            enum NavigationStatusEnum
            {
                navigation_status_idle                  = yarp::os::createVocab32('i', 'd', 'l', 'e'),
                navigation_status_preparing_before_move = yarp::os::createVocab32('p', 'r', 'e', 'p'),
                navigation_status_moving                = yarp::os::createVocab32('m', 'o', 'v', 'g'),
                navigation_status_waiting_obstacle      = yarp::os::createVocab32('w', 'a', 'i', 't'),
                navigation_status_goal_reached          = yarp::os::createVocab32('r', 'e', 'c', 'h'),
                navigation_status_aborted               = yarp::os::createVocab32('a', 'b', 'r', 't'),
                navigation_status_failing               = yarp::os::createVocab32('f', 'a', 'i', 'l'),
                navigation_status_paused                = yarp::os::createVocab32('p', 'a', 'u', 's'),
                navigation_status_thinking              = yarp::os::createVocab32('t', 'h', 'n', 'k'),
                navigation_status_error                 = yarp::os::createVocab32('e', 'r', 'r')
            };

            enum NavigationMapTypeEnum
            {
                global_map                              = yarp::os::createVocab32('g', 'l', 'o', 'b'),
                local_map                               = yarp::os::createVocab32('l', 'o', 'c', 'a')
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
            }
        }
    }
}

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
     * @return true/false
     */
    virtual bool gotoTargetByAbsoluteLocation(yarp::dev::Nav2D::Map2DLocation loc) = 0;

    /**
     * Gets the last navigation target in the world reference frame
     * @param loc the location of the robot
     * @return true/false
     */
    virtual bool getAbsoluteLocationOfCurrentTarget(yarp::dev::Nav2D::Map2DLocation& loc) = 0;

    /**
     * Ask the robot to reach a position defined in the robot reference frame. The final orientation of the goal is unspecified.
     * @param x
     * @param y
     * @return true/false
     */
    virtual bool gotoTargetByRelativeLocation(double x, double y) = 0;

    /**
     * Ask the robot to reach a position defined in the robot reference frame
     * @param x
     * @param y
     * @param theta
     * @return true/false
     */
    virtual bool gotoTargetByRelativeLocation(double x, double y, double theta) = 0;

    /**
     * Gets the last navigation target in the robot reference frame
     * @param x
     * @param y
     * @param theta
     * @return true/false
     */
    virtual bool getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta) = 0;

    /**
     * Apply a velocity command. velocities are expressed in the robot reference frame
     * @param x [m/s]
     * @param y [m/s]
     * @param theta [deg/s]
     * @param timeout The velocity command expires after the specified amount of time (by default 0.1 seconds)
     * @return true/false
     */
    virtual bool applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout = 0.1) = 0;
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
     * @return true/false
     */
    virtual bool getNavigationStatus(NavigationStatusEnum& status) = 0;

    /**
     * Terminates the current navigation task. Cannot be resumed.
     * @return true/false
     */
    virtual bool stopNavigation() = 0;

    /**
     * Ask to the robot to suspend the current navigation task for a defined amount of time. Can be resumed by resume().
     * @param time_s
     * @return true/false
     */
    virtual bool suspendNavigation(const double time_s= std::numeric_limits<double>::infinity()) = 0;

    /**
     * Resume a previously suspended navigation task.
     * @return true/false
     */
    virtual bool resumeNavigation() = 0;

    /**
     * Forces the navigation system to recompute the path from the current robot position to the current goal.
     * If no goal has been set, the command has no effect.
     * @return true/false
     */
    virtual bool recomputeCurrentNavigationPath() = 0;

    /**
     * Returns the list of waypoints generated by the navigation algorithm
     * @param trajectory_type specifies if we are requesting the waypoints of the global trajectory or the waypoints of the local trajectory
     * @param waypoints the list of waypoints generated by the navigation algorithm
     * @return true/false
     */
    virtual bool getAllNavigationWaypoints(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type, yarp::dev::Nav2D::Map2DPath& waypoints) = 0;

    /**
     * Returns the current waypoint pursued by the navigation algorithm
     * @param curr_waypoint the current waypoint pursued by the navigation algorithm
     * @return true/false
     */
    virtual bool getCurrentNavigationWaypoint(yarp::dev::Nav2D::Map2DLocation& curr_waypoint) = 0;

    /**
     * Returns the current navigation map processed by the navigation algorithm
     * @param map_type the map to be requested (e.g. global, local, etc.)
     * @param map the map, currently used by the navigation algorithm
     * @return true/false
     */
    virtual bool getCurrentNavigationMap(yarp::dev::Nav2D::NavigationMapTypeEnum map_type, yarp::dev::Nav2D::MapGrid2D& map) = 0;
};

/**
 * @ingroup dev_iface_navigation
 *
 * An interface to control the navigation of a mobile robot in a 2D environment.
 */
class YARP_dev_API yarp::dev::Nav2D::INavigation2D :  public yarp::dev::Nav2D::INavigation2DTargetActions,
                                                      public yarp::dev::Nav2D::INavigation2DControlActions,
                                                      public yarp::dev::Nav2D::ILocalization2D
{
public:
    /**
     * Destructor.
     */
    virtual ~INavigation2D() {}

    /**
     * Ask the robot to reach a previously stored location/area
     * @param location_name the name of a location/area previously saved
     * @return true/false
     */
    virtual bool gotoTargetByLocationName(std::string location_or_area_name) = 0;

    /**
     * Check if the robot is currently inside the specified area
     * @param area_name the name of an area previously saved
     * @return true/false
     */
    virtual bool checkInsideArea(std::string area_name) = 0;

    /**
     * Check if the robot is currently inside the specified area
     * @param area the area to be checked
     * @return true/false
     */
    virtual bool checkInsideArea (Nav2D::Map2DArea area) = 0;

    /**
     * Check if the robot is currently near to the specified area
     * @param loc the location to be checked
     * @param linear_tolerance linear tolerance [m]
     * @param angular_tolerance [deg 0-360]
     * @return true/false
     */
    virtual bool checkNearToLocation(Nav2D::Map2DLocation loc, double linear_tolerance, double angular_tolerance = std::numeric_limits<double>::infinity()) = 0;

    /**
     * Check if the robot is currently near to the specified area
     * @param location_name the name of the location: it will be searched in the server
     * @param linear_tolerance linear tolerance [m]
     * @param angular_tolerance [deg]
     * @return true/false
     */
    virtual bool checkNearToLocation(std::string location_name, double linear_tolerance, double angular_tolerance = std::numeric_limits<double>::infinity()) = 0;

    /**
     * Gets the name of the current target, if available (set by gotoTargetByLocationName)
     * @param location_name the name of the current target
     * @return true/false
     */
    virtual bool getNameOfCurrentTarget(std::string& location_name) = 0;

    /**
     * Store the current location of the robot
     * @param location_name the name of the location
     * @return true/false
     */
    virtual bool storeCurrentPosition(std::string location_name) = 0;

    /**
     * Store a location specified by the user in the world reference frame
     * @param location_name the name of the location
     * @param loc the location of the robot
     * @return true/false
     */
    virtual bool storeLocation(std::string location_name, Nav2D::Map2DLocation loc) = 0;

    /**
     * Retrieves a location previously stored by the user
     * @param location_name the name of the location
     * @param loc the location on the map
     * @return true/false
     */
    virtual bool getLocation(std::string location_name, Nav2D::Map2DLocation& loc) = 0;


    /**
     * Retrieves an area previously stored by the user
     * @param area_name the name of the area
     * @param area the area on the map
     * @return true/false
     */
    virtual bool getArea(std::string area_name, Nav2D::Map2DArea& area) = 0;

    /**
     * Get a list of all stored locations
     * @param the returned list of locations
     * @return true/false
     */
    virtual bool getLocationsList(std::vector<std::string>& locations) = 0;

    /**
     * Delete a location
     * @param location_name the name of the location
     * @return true/false
     */
    virtual bool deleteLocation(std::string location_name) = 0;

    /**
     * Delete all stored locations
     * @return true/false
     */
    virtual bool clearAllLocations() = 0;
};

constexpr yarp::conf::vocab32_t VOCAB_NAV_STOP                     = yarp::os::createVocab32('s', 't', 'o', 'p');
constexpr yarp::conf::vocab32_t VOCAB_NAV_SUSPEND                  = yarp::os::createVocab32('s', 'u', 's', 'p');
constexpr yarp::conf::vocab32_t VOCAB_NAV_RESUME                   = yarp::os::createVocab32('r', 'e', 's', 'm');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_NAVIGATION_WAYPOINTS = yarp::os::createVocab32('w', 'a', 'y', 's');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_CURRENT_WAYPOINT     = yarp::os::createVocab32('w', 'a', 'y');
constexpr yarp::conf::vocab32_t VOCAB_NAV_GET_NAV_MAP              = yarp::os::createVocab32('n', 'm', 'a', 'p');

#endif // YARP_DEV_INAVIGATION2D_H
