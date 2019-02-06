/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_INAVIGATION2D_H
#define YARP_DEV_INAVIGATION2D_H

#include <yarp/os/Vocab.h>
#include <yarp/os/Log.h>
#include <yarp/dev/ILocalization2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <vector>

namespace yarp {
    namespace dev {
        class INavigation2D;

        enum NavigationStatusEnum
        {
            navigation_status_idle                  = yarp::os::createVocab('i', 'd', 'l', 'e'),
            navigation_status_preparing_before_move = yarp::os::createVocab('p', 'r', 'e', 'p'),
            navigation_status_moving                = yarp::os::createVocab('m', 'o', 'v', 'g'),
            navigation_status_waiting_obstacle      = yarp::os::createVocab('w', 'a', 'i', 't'),
            navigation_status_goal_reached          = yarp::os::createVocab('r', 'e', 'c', 'h'),
            navigation_status_aborted               = yarp::os::createVocab('a', 'b', 'r', 't'),
            navigation_status_failing               = yarp::os::createVocab('f', 'a', 'i', 'l'),
            navigation_status_paused                = yarp::os::createVocab('p', 'a', 'u', 's'),
            navigation_status_thinking              = yarp::os::createVocab('t', 'h', 'n', 'k'),
            navigation_status_error                 = yarp::os::createVocab('e', 'r', 'r'),
        };
      }
}

/**
 * @ingroup dev_iface_navigation
 *
 * An interface to control the navigation of a mobile robot in a 2D environment.
 */
class yarp::dev::INavigation2D : public yarp::dev::ILocalization2D
{
public:
    /**
     * Destructor.
     */
    virtual ~INavigation2D() {}

    /**
     * Ask the robot to reach a position defined in the world reference frame
     * @param loc the location to be reached
     * @return true/false
     */
    virtual bool gotoTargetByAbsoluteLocation(yarp::dev::Map2DLocation loc) = 0;

    /**
     * Ask the robot to reach a previously stored location
     * @param location_name the name of a location previously saved
     * @return true/false
     */
    virtual bool gotoTargetByLocationName(std::string location_name) = 0;

    /**
    * Ask the robot to reach a position defined in the robot reference frame
    * @param x
    * @param y
    * @param theta
    * @return true/false
    */
    virtual bool gotoTargetByRelativeLocation(double x, double y, double theta) = 0;

    /**
    * Gets the last navigation target in the world reference frame
    * @param loc the location of the robot
    * @return true/false
    */
    virtual bool getAbsoluteLocationOfCurrentTarget(Map2DLocation& loc) = 0;

    /**
    * Gets the last navigation target in the robot reference frame
    * @param x
    * @param y
    * @param theta
    * @return true/false
    */
    virtual bool getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta) = 0;

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
    virtual bool storeLocation(std::string location_name, Map2DLocation loc) = 0;

    /**
    * Retrieves a location specified by the user in the world reference frame
    * @param location_name the name of the location
    * @param loc the location of the robot
    * @return true/false
    */
    virtual bool getLocation(std::string location_name, Map2DLocation& loc) = 0;

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
    * Ask to the robot to suspend the current navigation task. Can be resumed by resume()
    * @return true/false
    */
    virtual bool suspendNavigation() = 0;

    /**
    * Resume a previously suspended navigation task.
    * @return true/false
    */
    virtual bool resumeNavigation() = 0;
};




#endif // YARP_DEV_INAVIGATION2D_H
